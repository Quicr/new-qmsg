
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "Network.h"
#include "message_loop.h"

// Handy abstraction to store pipes and so on
struct NetworkProcess
{
    explicit NetworkProcess(const std::string& server, uint16_t port)
    : network(server, port)
    {
        sec2netFD = open( "/tmp/pipe-s2n", O_RDONLY, O_NONBLOCK );
        assert( sec2netFD >= 0 );
        fprintf(stderr, "NET: Got pipe from secProc\n");

        net2secFD = open( "/tmp/pipe-n2s", O_WRONLY, O_NONBLOCK );
        assert( net2secFD >= 0 );
        fprintf(stderr, "NET: Got pipe to netProc\n");

    }

    bool process_net_message(QMsgNetMessage& message);
    Network network;
    int sec2netFD = -1;
    int net2secFD = -1;
};

bool NetworkProcess::process_net_message(QMsgNetMessage& message)
{
    switch (message.type)
    {
        case QMsgNetSendASCIIMessage:
        {
            // publish message for this device
            const auto &msg = message.u.send_ascii_message;
            if(!msg.message.data) {
                // log
                break;
            }
            auto data = quicr::bytes(msg.message.data, msg.message.data + msg.message.length);
            network.publish(msg.team_id, msg.channel_id, msg.device_id, std::move(data));
        }
            break;
        case QMsgNetWatchDevices:
        {
            // send subscribes
            const auto &msg = message.u.watch_devices;
            if (msg.device_list.num_devices == 0 || !msg.device_list.device_list)
            {
                // log error
                break;
            }

            std::vector<uint16_t> devices;
            for (size_t i = 0; i < msg.device_list.num_devices; i++)
            {
                devices.push_back(msg.device_list.device_list[i]);
            }

            network.subscribe_to_devices(msg.team_id, msg.channel_id, std::move(devices));

        }
            break;
        case QMsgNetUnwatchDevices:
        {
            // send unsubscribe
            const auto &msg = message.u.unwatch_devices;
            if (msg.device_list.num_devices == 0 || !msg.device_list.device_list)
            {
                // log error
                break;
            }
            for (size_t i = 0; i < msg.device_list.num_devices; i++)
            {
                network.unsubscribe_from_device(msg.team_id, msg.channel_id, msg.device_list.device_list[i]);
            }
        }
            break;
        case QMsgNetMLSSignatureHash:
        {
            // subscribe to keypackage based on the hash
            auto &msg = message.u.mls_signature_hash;
            if (!msg.hash.data)
            {
                // log
                break;
            }
            auto kp_hash = quicr::bytes(msg.hash.data, msg.hash.data + msg.hash.length);
            network.subscribe_for_keypackage(msg.team_id, std::move(kp_hash));
        }
            break;
        case  QMsgNetMLSKeyPackage:
            // post key package for  this device
        {
            auto& msg = message.u.mls_key_package;
            if(!msg.key_package.data || !msg.key_package_hash.data) {
                // log error
                break;
            }

            auto kp = quicr::bytes(msg.key_package.data, msg.key_package.data + msg.key_package.length);
            auto kp_hash = quicr::bytes(msg.key_package_hash.data, msg.key_package_hash.data + msg.key_package_hash.length);

            network.handleKeyPackageEvent(EventSource::SecProc, msg.team_id, std::move(kp), std::move(kp_hash));
        }
            break;
        case QMsgNetMLSWelcome:
            // publish welcome computed by the leader
        {
            auto& msg = message.u.mls_welcome;
            if(!msg.welcome.data) {
                // log error
                break;
            }
            auto welcome = quicr::bytes(msg.welcome.data, msg.welcome.data + msg.welcome.length);
            network.handleMLSWelcomeEvent(EventSource::SecProc, msg.team_id, std::move(welcome));
        }
            break;
        case QMsgNetMLSCommit:
            // publish commit computed by the leader
        {
            auto& msg = message.u.mls_commit;
            if(!msg.commit.data) {
                // log error
                break;
            }
            auto commit = quicr::bytes(msg.commit.data, msg.commit.data + msg.commit.length);
            network.handleMLSCommitEvent(EventSource::SecProc, msg.team_id, std::move(commit));
        }
            break;
        default:
            fprintf(stderr, "unknown message type");
    }

    return true;
}

int main( int argc, char* argv[]){

  fprintf(stderr, "NET: Starting netProc\n");
  // setup our network layer
  auto network = Network("127.0.0.1", 7777);

  // set up connectors to the network process
  NetworkProcess network_process {"127.0.0.1", 7777};

  // setup MessageLoop
  auto message_loop = MessageLoop{};
  message_loop.read_from_fd = network_process.sec2netFD;
  message_loop.keep_processing = true;
  message_loop.process_net_message_fn = std::bind(&NetworkProcess::process_net_message,
                                                  &network_process,
                                                  std::placeholders::_1);

  // kick-off the message loop
  auto err = message_loop.process(8192);

  if (err != LoopProcessResult::SUCCESS) {
      // log and fail
  }

  return 0;
}

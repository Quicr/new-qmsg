
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
        if (QMsgEncoderInit(&context))
        {
            // log an error and...
            assert(0);
        }
    }

    bool process_net_message(QMsgNetMessage& message, EventSource source, quicr::bytes&& message_raw);
    void perform_network_io();
    void writeToSecProc(quicr::bytes&& message);

    Network network;
    int sec2netFD = -1;
    int net2secFD = -1;
    QMsgEncoderContext *context;
};

void NetworkProcess::writeToSecProc(quicr::bytes&& message)
{
    std::cout << "Writing to secproc:" << message.size() << " bytes" << std::endl;
    write( net2secFD, message.data(), message.size());
}

bool NetworkProcess::process_net_message(QMsgNetMessage& message, EventSource source, quicr::bytes&& message_raw)
{
    switch (message.type)
    {
        case QMsgNetSendASCIIMessage:
        {
            std::cout << "[NetMsgLoop]:Got QMsgNetSendASCIIMessage " << std::endl;

            // publish message for this device
            const auto &msg = message.u.send_ascii_message;
            if(!msg.message.data) {
                std::cout << "[NetMsgLoop]:QMsgNetSendASCIIMessage: Missing Data" << std::endl;
                break;
            }

            (source == EventSource::SecProc)
                ? network.publish(msg.team_id, msg.channel_id, msg.device_id, std::move(message_raw))
                    : writeToSecProc(std::move(message_raw));
        }
            break;
        case QMsgNetWatchDevices:
        {
            std::cout << "[NetMsgLoop]:Got QMsgNetWatchDevices " << std::endl;
            // send subscribes
            const auto &msg = message.u.watch_devices;
            if (msg.device_list.num_devices == 0 || !msg.device_list.device_list)
            {
                std::cout << "[NetMsgLoop]: QMsgNetWatchDevices: Device list malformed" << std::endl;
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
            std::cout << "[NetMsgLoop]:Got QMsgNetMLSSignatureHash " << std::endl;
            // subscribe to keypackage based on the hash
            auto &msg = message.u.mls_signature_hash;
            if (!msg.hash.data)
            {
                std::cout << "[NetMsgLoop]:QMsgNetMLSSignatureHash: malformed hash" << std::endl;
                break;
            }

            auto kp_hash = quicr::bytes(msg.hash.data, msg.hash.data + msg.hash.length);
            network.subscribe_for_keypackage(msg.team_id, std::move(kp_hash));
        }
            break;
        case  QMsgNetMLSKeyPackage:
            // post key package for  this device
        {
            std::cout << "[NetMsgLoop]:Got QMsgNetMLSKeyPackage " << std::endl;
            auto& msg = message.u.mls_key_package;
            if(!msg.key_package.data || !msg.key_package_hash.data) {
                std::cout << "[NetMsgLoop]: Invalid QMsgNetMLSKeyPackage " << std::endl;
                break;
            }

            auto kp_hash = quicr::bytes(msg.key_package_hash.data, msg.key_package_hash.data + msg.key_package_hash.length);
            (source == EventSource::SecProc) ?
                network.handleKeyPackageEvent(source, msg.team_id, std::move(message_raw), std::move(kp_hash))
                    : writeToSecProc(std::move(message_raw));
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
            (source == EventSource::SecProc) ?
                network.handleMLSWelcomeEvent(source, msg.team_id, std::move(message_raw))
                    : writeToSecProc(std::move(message_raw));
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
            (source == EventSource::SecProc) ?
                network.handleMLSCommitEvent(source, msg.team_id, std::move(message_raw))
                    : writeToSecProc(std::move(message_raw));
        }
            break;
        default:
            fprintf(stderr, "unknown message type");
    }

    return true;
}

void NetworkProcess::perform_network_io()
{
    auto incoming_messages = std::vector<QuicrMessageInfo>{};
    network.check_network_messages(incoming_messages);
    // decode and process  - todo move this into the message_loop as well
    for(auto& message : incoming_messages) {
        QMsgNetMessage qMsgNetMessage {};
        size_t consumed = 0;
        auto qmsg_enc_result = QMsgNetDecodeMessage(context,
                                                    reinterpret_cast<uint8_t *>(message.data.data()),
                                                    message.data.size(),
                                                    &qMsgNetMessage,
                                                    &consumed);

        if(qmsg_enc_result != QMsgEncoderSuccess) {
            std::cout << "[NetworkIO]: Decoder Failed " << qmsg_enc_result << ", consumed: " << consumed << std::endl;
            continue;
        }

        process_net_message(qMsgNetMessage, EventSource::Network, std::move(message.data));

    }
}

int main( int argc, char* argv[]) {


    if(argc < 2) {
        std::cerr << "Usage: <program> user-name" << std::endl;
        std::cerr << "user-name: name of the user" << std::endl;
        exit(0);
    }

    std::string user;
    user.assign(argv[1]);
    if(user.empty()) {
        std::cerr << "empty user input";
        exit(-1);
    }

    fprintf(stderr, "NET: Starting netProc\n");
    std::string pipe_name = std::string("/tmp/pipe-s2n-") + user;
    std::cout << "Checking for pipe from secProc %s" << pipe_name << std::endl;
    int sec2netFD = open( pipe_name.c_str(), O_RDONLY, O_NONBLOCK );
    assert( sec2netFD >= 0 );
    std::cout << "NET: Got pipe from secProc %s" << pipe_name << std::endl;

    pipe_name = std::string("/tmp/pipe-n2s-") + user;
    std::cout << "Checking for pipe to netProc " << pipe_name << std::endl;
    int net2secFD = open( pipe_name.c_str(), O_WRONLY, O_NONBLOCK );
    assert( net2secFD >= 0 );
    std::cout << "NET: Got pipe to netProc " << pipe_name << std::endl;

  // set up connectors to the network process
  NetworkProcess network_process {"127.0.0.1", 7777};
  network_process.sec2netFD  = sec2netFD;
  network_process.net2secFD = net2secFD;

  //network_process.network.start();

  fprintf(stderr, "NET: Starting message loop\n");

    // setup MessageLoop
  auto message_loop = MessageLoop{};
  message_loop.read_from_fd = network_process.sec2netFD;
  message_loop.keep_processing = true;
  message_loop.process_net_message_fn = std::bind(&NetworkProcess::process_net_message,
                                                  &network_process,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  std::placeholders::_3);

  message_loop.loop_fn = std::bind(&NetworkProcess::perform_network_io,
                                   &network_process);

  // kick-off the message loop
  auto err = message_loop.process(8192);

  if (err != LoopProcessResult::SUCCESS) {
      // log and fail
  }

  return 0;
}

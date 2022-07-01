
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include <future>
#include <thread>
#include <chrono>

#include "message_loop.h"

// Handy abstraction to store pipes and so on
// This is wired up in a short time to test the flow
// There is a lot of repetition which is intentional
struct FakeSecProcess
{
    QMsgEncoderContext *context;
    int sec2netFD = -1;
    int net2secFD = -1;
    bool is_leader = false;

    FakeSecProcess(const std::string& user)
    {
        std::string pipe_name = std::string("/tmp/pipe-s2n-") + user;
        fprintf(stderr, "Checkin for pipe to netProc : %s\n", pipe_name.c_str());
        net2secFD = open( pipe_name.c_str(), O_WRONLY, O_NONBLOCK );
        assert( net2secFD >= 0 );
        fprintf(stderr, "Got pipe to netProc : %s\n", pipe_name.c_str());

        pipe_name = std::string("/tmp/pipe-n2s-") + user;
        fprintf(stderr, "Checking for  pipe from Fake secProc %s\n", pipe_name.c_str());
        sec2netFD = open( pipe_name.c_str(), O_RDONLY, O_NONBLOCK );
        assert( sec2netFD >= 0 );
        fprintf(stderr, "Got pipe from Fake secProc %s\n", pipe_name.c_str());

        if (QMsgEncoderInit(&context))
        {
            assert(0);
        }

    }

    void send_kp_hash() {
        // send to net
        QMsgNetMessage message {};
        char kp_hash[] = "Bob's KP Hash";
        std::uint32_t hash_length = strlen(kp_hash);

        message.type = QMsgNetMLSSignatureHash;
        message.u.mls_signature_hash.team_id = 1000;
        message.u.mls_signature_hash.hash.length = hash_length;
        message.u.mls_signature_hash.hash.data =
                reinterpret_cast<std::uint8_t *>(kp_hash);
        std::memcpy(message.u.mls_signature_hash.hash.data,
                    kp_hash,
                    message.u.mls_signature_hash.hash.length);
        std::size_t encoded_length;
        char data_buffer[1500];
        auto encode_result = QMsgNetEncodeMessage(context,
                                       &message,
                                       data_buffer,
                                       sizeof(data_buffer),
                                       &encoded_length);
        assert(encode_result == QMsgEncoderSuccess);
        std::cerr << "Sending KP Hash message" << std::endl;
        // write to the pipe
        write(net2secFD, data_buffer, encoded_length);
    }

    void send_key_package() {
        // send to net
        QMsgNetMessage message {};
        char kp[] = "Bob's Key Package";
        std::uint32_t kp_length = strlen(kp);
        char kp_hash[] = "Bob's KP Hash";
        std::uint32_t hash_length = strlen(kp_hash);

        message.type = QMsgNetMLSKeyPackage;
        message.u.mls_key_package.team_id = 1000;
        message.u.mls_key_package.key_package_hash.length = hash_length;
        message.u.mls_key_package.key_package_hash.data =
                reinterpret_cast<std::uint8_t *>(kp_hash);
        std::memcpy(message.u.mls_key_package.key_package_hash.data,
                    kp_hash,
                    message.u.mls_key_package.key_package_hash.length);
        message.u.mls_key_package.key_package.length = kp_length;
        message.u.mls_key_package.key_package.data =
                reinterpret_cast<std::uint8_t *>(kp);
        std::memcpy(message.u.mls_key_package.key_package.data,
                    kp,
                    message.u.mls_key_package.key_package.length);

        std::size_t encoded_length;
        char data_buffer[1500];
        auto encode_result = QMsgNetEncodeMessage(context,
                                                  &message,
                                                  data_buffer,
                                                  sizeof(data_buffer),
                                                  &encoded_length);

        assert(encode_result == QMsgEncoderSuccess);
        std::cerr << "Sending KP  message" << std::endl;
        // write to the pipe
        write(net2secFD, data_buffer, encoded_length);
    }

    void send_watch(uint32_t team, uint32_t channel, uint16_t device) {
        std::cout << "Sending Watch for team:" << team << ",Channel:" << channel << ", Device" << device << std::endl;
        QMsgNetMessage message{};
        QMsgDeviceID devices[] = {1};

        message.type = QMsgNetWatchDevices;
        message.u.watch_devices.team_id = 1000;
        message.u.watch_devices.channel_id = 2;
        message.u.watch_devices.device_list.num_devices = 1;
        message.u.watch_devices.device_list.device_list = devices;

        std::size_t encoded_length;
        char data_buffer[1500];
        auto result = QMsgNetEncodeMessage(context,
                                           &message,
                                           data_buffer,
                                           sizeof(data_buffer),
                                           &encoded_length);

        write(net2secFD, data_buffer, encoded_length);
    }


    void send_ascii_message(const std::string& ascii_message, uint32_t team, uint32_t channel, uint16_t device) {
        std::cout << "Sending Ascii for team:" << team << ",Channel:" << channel << ", Device" << device << std::endl;

        QMsgUIMessage message{};
        std::uint32_t string_length = ascii_message.length();

        message.type = QMsgUISendASCIIMessage;
        message.u.send_ascii_message.team_id = 1000;
        message.u.send_ascii_message.channel_id = 2;
        message.u.send_ascii_message.message.length = string_length;
        message.u.send_ascii_message.message.data =
                reinterpret_cast<std::uint8_t *>(const_cast<char*>(ascii_message.c_str()));
        std::memcpy(message.u.send_ascii_message.message.data,
                    ascii_message.c_str(),
                    message.u.send_ascii_message.message.length);

        std::size_t encoded_length;
        char data_buffer[1500];
        auto result = QMsgUIEncodeMessage(context,
                                          &message,
                                          data_buffer,
                                          sizeof(data_buffer),
                                          &encoded_length);

        write(net2secFD, data_buffer, encoded_length);
    }

    bool process_net_message(QMsgNetMessage& message, EventSource source, quicr::bytes&& message_raw)
    {
        switch (message.type)
        {
            case  QMsgNetMLSKeyPackage:
                std::cout << "Leader received the key package at leader?" << is_leader << std::endl;
                // subscribe to team-1000, channel-2, device-1
                send_watch(1000, 2, 1);
                break;
            default:
                fprintf(stderr, "unknown message type");
        }
        return true;
    }

    static std::string read_input() {
        std::cout << "Enter QMsg Chat Input\n";
        std::string input;
        std::cin >> input;
        return input;
    }

    void io_loop() {
        std::future<std::string> future = std::async(read_input);
        std::string input;
        if(future.wait_for(std::chrono::seconds(5)) == std::future_status::ready) {
            input = future.get();
        }

        if(!input.empty()) {
            send_ascii_message(input);
        }
    }

};

int main( int argc, char* argv[]){

  if(argc < 3) {
      std::cerr << "Usage: <program> user-name leader" << std::endl;
      std::cerr << "user-name: nme of the user" << std::endl;
      std::cerr << "leader: true or false" << std::endl;
      exit(0);
  }

  std::string user;
  user.assign(argv[1]);
  if(user.empty()) {
      std::cerr << "empty user input";
      exit(-1);
  }

  bool is_leader = false;
  std::string leader_in;
  leader_in.assign(argv[2]);
  if (leader_in == "true") {
      is_leader = true;
  }

  fprintf(stderr, "NET: Starting SecProcess as leader ? %d\n", is_leader);

  FakeSecProcess secProc{user};
  secProc.is_leader = is_leader;

  if(is_leader) {
      std::cout << "Sec: send mls_signature_hash: to net to subscribe for key_package" << std::endl;
      secProc.send_kp_hash();
  } else {
      std::cout << "Sec: send mls_key_package: to net to be forwarded to the leader " << std::endl;
      secProc.send_key_package();
  }


  auto message_loop = MessageLoop{};
  message_loop.read_from_fd = secProc.sec2netFD;
  message_loop.keep_processing = true;
  message_loop.process_net_message_fn = std::bind(&FakeSecProcess::process_net_message,
                                                    &secProc,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2,
                                                    std::placeholders::_3);

    message_loop.loop_fn = std::bind(&FakeSecProcess::io_loop,
                                     &secProc);

    // kick-off the message loop
    auto err = message_loop.process(8192);

    if (err != LoopProcessResult::SUCCESS) {
        // log and fail
    }


  return 0;
}

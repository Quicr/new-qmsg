
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>

#include "message_loop.h"

// Handy abstraction to store pipes and so on
struct FakeSecProcess
{
    QMsgEncoderContext *context;
    int sec2netFD = -1;
    int net2secFD = -1;


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

  // secProc.send_kp_hash();

  while(1)
  {
        std::cout << "Waiting for input" << std::endl;
        std::string input;
        std::cin >> input;
        if(input == "kp_hash") {
            secProc.send_kp_hash();
        }

        if (input == "key_package") {
            secProc.send_key_package();
        }
  }

  return 0;
}


#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "Network.h"

#include "qmsg/encoder.h"
#include "qmsg/net_types.h"

// Handy abstraction to store pipes and so on
struct NetworkProcess
{
    NetworkProcess() {
        sec2netFD = open( "/tmp/pipe-s2n", O_RDONLY, O_NONBLOCK );
        assert( sec2netFD >= 0 );
        fprintf(stderr, "NET: Got pipe from secProc\n");

        net2secFD = open( "/tmp/pipe-n2s", O_WRONLY, O_NONBLOCK );
        assert( net2secFD >= 0 );
        fprintf(stderr, "NET: Got pipe to netProc\n");

    }

    int sec2netFD = -1;
    int net2secFD = -1;
};



void ProcessingLoop()
{
    const int bufSize = 8192;
    char secBuf[bufSize];
    size_t fragment_size = 0;
    QMsgEncoderResult qmsg_enc_result;
    QMsgEncoderContext *context;
    size_t consumed;
    size_t total_consumed;
    QMsgNetMessage message;

    if (QMsgEncoderInit(&context))
    {
        // log an error and...
        return;
    }

    while(keep_processing)
    {
        int numSelectFD = select(maxFD + 1, &fdSet, nullptr, nullptr, &timeout);
        assert(numSelectFD >= 0);

        // process secProc
        if ((network_process.sec2netFD > 0) &&
            (FD_ISSET(network_process.sec2netFD, &fdSet)))
        {
            ssize_t num = read(network_process.sec2netFD,
                               secBuf + fragment_size,
                               bufSize - fragment_size);
            // Update the total bytes to include the previous fragment
            num += fragment_size;

            // Reset the fragment length
            fragment_size = 0;

            // Set the total consumed to 0
            total_consumed = 0;

            // Process as many messages in the buffer as possible
            do
            {
                qmsg_enc_result = QMsgNetDecodeMessage(context,
                                                       secBuf + total_consumed,
                                                       num - total_consumed,
                                                       &message,
                                                       &consumed);

                // Update the total octets consumed
                total_consumed += consumed;

                if (qmsg_enc_result == QMsgEncoderSuccess)
                {

                    // Do something with this message
                }

                if ((qmsg_enc_result == QMsgEncoderInvalidMessage) ||
                    (qmsg_enc_result == QMsgEncoderCorruptMessage))
                {
                    /// Just log the fact the message was invalid or corrupu; it will get skipped over
                }
            } while ((total_consumed < num) && (consumed > 0);

            // If there is partial data in the buffer, move it to the front
            // and take note of the new fragment size
            if (total_consumed < num)
            {
                memmove(secBuf, secBuf + total_consumed, num - total_consumed);
                fragment_size = num - total_consumed;
            }

            /* Notes on these errors...
            *   QMsgEncoderUnknownError,  ... this is generally a malloc() failure; it's not recoverable
            *   QMsgEncoderShortBuffer,   ... ths means we tried to read beyond the buffer; that total_consumed will take care of this
            *   QMsgEncoderInvalidContext, ... this should never happen so long as init() was successful, but perhaps log an error and exit?
            *   QMsgEncoderInvalidMessage, ... this means the message it bad corrupt (it gets ignored in the loop above)
            *   QMsgEncoderCorruptMessage  ... The message is corrupt
            */
        }
    }

    QMsgEncoderDeinit(context);
}

int main( int argc, char* argv[]){

  fprintf(stderr, "NET: Starting netProc\n");
  // set up connectors to the network process
  NetworkProcess network_process;

  // setup encoder/decoder context for messages
  QMsgEncoderContext *context = nullptr;
  QMsgEncoderInit(&context);

  auto network = Network("127.0.0.1", 7777);


  const int bufSize=8192;
  char secBuf[bufSize];
 
  while( true ) {
    // waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;

    FD_ZERO(&fdSet);
    FD_SET(network_process.sec2netFD, &fdSet);
    maxFD = (network_process.sec2netFD>maxFD) ? network_process.sec2netFD : maxFD;

    int numSelectFD = select( maxFD+1 , &fdSet , nullptr, nullptr, &timeout );
    assert( numSelectFD >= 0 );

    // process secProc
    if ( (network_process.sec2netFD > 0) && ( FD_ISSET(network_process.sec2netFD, &fdSet) ) ) {
      ssize_t num = read( network_process.sec2netFD, secBuf, bufSize );
      if ( num > 0 ) {
        fprintf( stderr, "NET: Read %d bytes from SecProc: ", (int)num );
        fwrite( secBuf, 1 , num , stderr );
        fprintf( stderr, "\n");

        // decode the message
        auto  message = QMsgNetMessage{};
        std::size_t bytes_consumed{};
        auto result = QMsgNetDecodeMessage(context, secBuf, num, &message, &bytes_consumed);
        if (result != QMsgEncoderResult::QMsgEncoderSuccess) {
            fprintf(stderr, "NET: Message Decoder Error %d", result);
            continue;
        }

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
                network.handleMLSWelcomeEvent(EventSource::SecProc, msg.team_id, std::move(commit));
            }
                break;
            default:
                fprintf(stderr, "unknown message type");
        }

        // send to UI processor - this has to move out from this loop
        const char* prefix ="reply: ";
        write( net2secFD, prefix, strlen( prefix ) );
        write( net2secFD, secBuf, num );
      }
    }
  }

  QMsgEncoderDeinit(context)

  return 0;
}

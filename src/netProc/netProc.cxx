
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

int main( int argc, char* argv[]){

  fprintf(stderr, "NET: Starting netProc\n");
  // set up connectors to the network process
  NetworkProcess network_process;

  // setup encoder/decoder context for messages
  QMsgEncoderContext *context = nullptr;
  QMsgEncoderInit(&context);

  auto network = Network("127.0.0.1", 7777);


  const int bufSize=128;
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

        /*
         * typedef enum QMsgNetMessageType
{
    QMsgNetInvalid = 0,
    QMsgNetReceiveASCIIMessage,
    QMsgNetMLSSignatureHash,
    QMsgNetMLSKeyPackage,
    QMsgNetMLSAddKeyPackage,
    QMsgNetMLSCommit,
    QMsgNetDeviceInfo,
    QMsgNet_RESERVED_RANGE
} QMsgNetMessageType;
         */
        switch (message.type)
        {
            case QMsgNetSendASCIIMessage:
                // publish message for this device

                break;
            case QMsgNetWatchDevices:
                // send subscribes
                break;
            case QMsgNetUnwatchDevices:
                // send unsbscribes
               break;
            case QMsgNetMLSSignatureHash:
                // subscribe to keypackage
               break;
            case  QMsgNetMLSKeyPackage:
                // post key package
                break;
            case QMsgNetMLSWelcome:
                // publish welcome
               break;
            case QMsgNetMLSCommit:
                // publish commit
                break;
            case QMsgNetDeviceInfo:
                // save device_id for enabling publishes
            default:
                fprintf(stderr, "unknown message type");
        }
        // send to UI processor
        const char* prefix ="reply: ";
        write( net2secFD, prefix, strlen( prefix ) );
        write( net2secFD, secBuf, num );
      }
    }
  }

  QMsgEncoderDeinit(context)

  return 0;
}

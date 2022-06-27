
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <thread>

#include <quicr/quicr_client.h>

using namespace quicr;

struct PubSubApp : QuicRClient::Delegate {

    void on_data_arrived(const std::string& name,
                         bytes&& data,
                         uint64_t group_id,
                         uint64_t object_id) override
    {
        std::cerr << "on_data_arrived: writing to sec processor now" << std::endl;
        const char* prefix ="reply: ";
        write( net2secFD, prefix, strlen( prefix ));
        write( net2secFD, data.data(), data.size());
    }

    void on_connection_close(const std::string& name)
    {
        std::cout << "consumer connection closed: " << name << "\n";
    }

    void log(LogLevel /*level*/, const std::string& message)
    {
        std::cerr << message << std::endl;
    }

    int net2secFD;
};

int main( int argc, char* argv[])
{
    // quicr channel name
    const std::string message_channel = "quicr://cto-team/innovation/netproc";
    PubSubApp delegate;
    QuicRClient qr {delegate, "127.0.0.1", 7777};
    qr.register_names(std::vector<std::string>{message_channel}, true);
    qr.subscribe(std::vector<std::string>{message_channel}, true, true);
    fprintf(stderr, "NET: Publishing on the name: %s", message_channel.data());
    while (!qr.is_transport_ready()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    fprintf(stderr, "NET: Starting netProc\n");

    int sec2netFD = open( "/tmp/pipe-s2n", O_RDONLY, O_NONBLOCK );
    assert( sec2netFD >= 0 );
    fprintf(stderr, "NET: Got pipe from secProc\n");

    int net2secFD = open( "/tmp/pipe-n2s", O_WRONLY, O_NONBLOCK );
    assert( net2secFD >= 0 );
    fprintf(stderr, "NET: Got pipe to netProc\n");

    // let the app know where to write the incoming data
    delegate.net2secFD = net2secFD;

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
        FD_SET(sec2netFD, &fdSet); maxFD = (sec2netFD>maxFD) ? sec2netFD : maxFD;
        int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
        assert( numSelectFD >= 0 );

        // processs secProc
        if ( (sec2netFD > 0) && ( FD_ISSET(sec2netFD, &fdSet) ) ) {
          //fprintf(stderr, "NET: Reding Sec Proc\n");
          ssize_t num = read( sec2netFD, secBuf, bufSize );
          if ( num > 0 ) {
            fprintf( stderr, "NET: Read %d bytes from SecProc: ", (int)num );
            fprintf( stderr, "NET: Publishing on %s: ", message_channel.data());
            qr.publish_named_data(message_channel, bytes(secBuf, secBuf+bufSize), 0, 0);
          }
        }
      }

    return 0;
}

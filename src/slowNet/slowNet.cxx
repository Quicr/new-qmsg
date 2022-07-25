
#include <assert.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <arpa/inet.h>

#include <slower.h>
#include <name.h>


#include "secApi.h"

class Relay {
private:
  SlowerConnection slower;
  SecApi& secApi;
public:
  Relay(  SecApi& secApiVal, const char* relayName=NULL ) : secApi( secApiVal ) {
    const char defaultRelayName[] = "relay.us-east-2.qmsg.ctgpoc.com";
    const int port = 5004;
    const int mask=16;
    
    int err = slowerSetup( slower  );
    assert( err == 0 );
    
    SlowerRemote relay;
    err = slowerRemote( relay , relayName?relayName:defaultRelayName, port );
    if ( err ) {
      std::cerr << "Could not lookup IP address for relay: " << relayName << std::endl;
    }
    assert( err == 0 );
    std::clog << "Using relay at " << inet_ntoa( relay.addr.sin_addr)  << ":" <<  ntohs(relay.addr.sin_port) << std::endl;
    
    err = slowerAddRelay( slower, relay );
    assert( err == 0 );
    
  }
  
  ~Relay() {
    slowerClose( slower );
  }

  int getFD() { return slowerGetFD( slower ); }
  
  void pub(const MsgShortName& name, const uint8_t* data, const int len ) {
    int err = slowerPub( slower,  name,  (char*) data , len  );
    assert( err == 0 );
  }

  void sub(const MsgShortName& name, const int mask ) {
    int err = slowerSub( slower,  name, mask  );
    assert( err == 0 );
  }

  void recv( ) {
    MsgHeader mhdr = {0};
    char buf[1024];
    int bufSize=sizeof(buf);
    int bufLen=0;
    int err = slowerRecvPub( slower, &mhdr, buf, bufSize, &bufLen );
    assert( err == 0 );
    Name name( mhdr.name );

    if ( bufLen > 0 ) {
      std::clog << "NET: Recv PUB "
                << name.longString() 
                << " len=" << bufLen 
                << std::endl;
      
      secApi.recvAsciiMsg( mhdr.name.spec.team, mhdr.name.spec.device, mhdr.name.spec.channel,
                           (uint8_t*)buf, bufLen );
    }
  }
};

int main( int argc, char* argv[]){

  std::clog <<   "NET: Starting netProc" << std::endl;
  SecApi secApi;

  Relay relay( secApi, getenv("SLOWR_RELAY") );

  const QMsgOrgID org=1; // TODO load from config 
  
  //  const int mask = 16;
  int msgNum=1; // TODO - make msgnum per team/device/ch
  
  
  while( true ) {
    //waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;
    FD_ZERO(&fdSet);
    {
      int sec2netFD = secApi.getReadFD();
      FD_SET(sec2netFD, &fdSet); maxFD = (sec2netFD>maxFD) ? sec2netFD : maxFD;
    }
    {
      int relayFD = relay.getFD();
      FD_SET(relayFD, &fdSet); maxFD = (relayFD>maxFD) ? relayFD : maxFD;
    }
    int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
    assert( numSelectFD >= 0 );

    // process relay
    if ( (numSelectFD > 0) && ( FD_ISSET( relay.getFD(), &fdSet) ) ) {
      relay.recv();
    }
    
    // processs secProc
    if ( (numSelectFD > 0) && ( FD_ISSET( secApi.getReadFD(), &fdSet) ) ) {
      //std::clog <<   "NET: Reading from SecProc" << std::endl;
     
      QMsgNetMessage message{};
      secApi.readMsg( &message );

      switch ( message.type ) {
      case QMsgNetWatchDevices:
        {
          std::clog << "NET: Got watch from SecProc:"
                    << " team=" <<    message.u.watch_devices.team_id
                  << " ch= " <<   message.u.watch_devices.channel_id
              << std::endl;
           // get a local copy before memory is invalid
         QMsgDeviceID* ptr =  message.u.watch_devices.device_list.device_list;
         QMsgLength len =   message.u.watch_devices.device_list.num_devices;
         std::vector<QMsgDeviceID> devList( ptr, ptr+len );
         std::clog << "   dev: ";
         for ( const auto& dev : devList ) {  std::clog << dev << " "; }
         std::clog << std::endl;
#if 1
         for ( const auto& dev : devList ) {
           assert( message.u.watch_devices.team_id > 0);
           assert( org >0  );
           assert( message.u.watch_devices.channel_id >0 );

           assert( message.u.watch_devices.team_id <= (1<<20) );
           assert( org <= (1<<18) );
           assert( message.u.watch_devices.channel_id < (1<<10) );
           //assert( dev < (1<<20) ); // TODO - move dev id to 32 bits 
                   
           Name name(  NamePath::message, org, message.u.watch_devices.team_id,
                       (uint16_t)message.u.watch_devices.channel_id , dev );
           std::clog << "   sub=" << name.longString() << std::endl;

           const int mask = (dev==0) ? 40 : 20;
           relay.sub( name.shortName() , mask );
         }
#endif
        } 
        break;
      case QMsgNetSendASCIIMessage:
        {
        std::clog << "NET: Got SendAsciiMsg from SecProc: "
          << " team=" <<   message.u.send_ascii_message.team_id
          << " device=" <<  message.u.send_ascii_message.device_id
          << " ch=" <<  message.u.send_ascii_message.channel_id
          << " val=" << message.u.send_ascii_message.message.length 
          << std::endl;

        

#if 1
        assert( message.u.watch_devices.channel_id <= 0xFFFF );
        Name name(  NamePath::message, org,
                    message.u.send_ascii_message.team_id,
                    (uint16_t)message.u.send_ascii_message.channel_id,
                    message.u.send_ascii_message.device_id,
                    msgNum++ // TODO - persiste msg nums 
                   );
        
          std::clog << "Pub to " << name.longString() << std::endl;
          relay.pub( name.shortName(),
                   message.u.send_ascii_message.message.data,
                   message.u.send_ascii_message.message.length );
#else
          secApi.recvAsciiMsg(  message.u.send_ascii_message.team_id,
                                message.u.send_ascii_message.device_id,
                                message.u.send_ascii_message.channel_id,
                                message.u.send_ascii_message.message.data,
                                message.u.send_ascii_message.message.length );
#endif    
        }
          break;
      case QMsgNetInvalid:
         std::clog << "NET: Got Invalid msg from SecProc: " << std::endl;
         break;
      default:
        assert(0);
      }
    }
  }
  
  return 0;
}

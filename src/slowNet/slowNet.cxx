
#include <assert.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <arpa/inet.h>

#include <slower.h>


#include "secApi.h"

class Name{
private:
  ShortName name;
public:
  Name( ShortName& n ) : name(n) {};
  Name( int team, int device, int channel, int msgNum=0 ){
    uint64_t t = team;
    uint64_t d = device;
    uint64_t c = channel;
    uint64_t n = msgNum;
    
    name.part[1] = (t<<32) + d;
    name.part[0] = (c<<32) + n;
  }

  int team()    { uint64_t r=name.part[1]; r = r>>32; return (int)(r); };
  int device()  { uint64_t r=name.part[1]; r = r>> 0; return (int)(r); };
  int channel() { uint64_t r=name.part[0]; r = r>>32; return (int)(r); };
  int msgNum()  { uint64_t r=name.part[0]; r = r>> 0; return (int)(r); };

  ShortName& sname() { return name; }
  std::string lname() {
    std::stringstream ss;
    ss << "team-" << team()
       << "/dev-" << device() 
       << "/ch-" << channel() ;
    if ( msgNum() == 0 ) {
      ss << "/*";
    }
    else {
      ss << "/msg-" << msgNum() ;
    }
    return ss.str();
  };
};


class Relay {
private:
  SlowerConnection slower;
  SecApi& secApi;
public:
  Relay(  SecApi& secApiVal ) : secApi( secApiVal ) {
    const char relayName[] = "relay.us-east-2.qmsg.ctgpoc.com";
    const int port = 5004;
    const int mask=16;
    
    int err = slowerSetup( slower, port  );
    assert( err == 0 );
    
    SlowerRemote relay;
    err = slowerRemote( relay , relayName, port );
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
  
  void pub( const ShortName& name, const uint8_t* data, const int len ) {
    int err = slowerPub( slower,  name,  (char*) data , len  );
    assert( err == 0 );
  }

  void sub( const ShortName& name, const int mask ) {
    int err = slowerSub( slower,  name, mask  );
    std::clog << "   sub " << std::hex << name.part[1] << "-" <<  name.part[0] << std::dec << std::endl;
    assert( err == 0 );
  }

  void recv( ) {
    ShortName shortName;
    char buf[1024];
    int bufSize=sizeof(buf);
    int bufLen=0;
    int err = slowerRecvPub( slower, &shortName, buf, bufSize, &bufLen );
    assert( err == 0 );
    Name name( shortName );
    
    if ( bufLen > 0 ) {
      std::clog << "Got data for name="
                << std::hex << shortName.part[1] << "-" <<  shortName.part[0] << std::dec 
                << " data=" ;
      for ( int i=0; i< bufLen; i++ ) {
        char c = buf[i];
        if (( c >= 32 ) && ( c <= 0x7e ) ) {
          std::clog << c;
        }
      }
      std::clog << std::endl;

      std::clog << "   name=" << name.lname() << std::endl;
      

      secApi.recvAsciiMsg( name.team(), name.device(), name.channel(),
                           (uint8_t*)buf, bufLen );
    }
  }
};

int main( int argc, char* argv[]){

  std::clog <<   "NET: Starting netProc" << std::endl;
  SecApi secApi;

  Relay relay( secApi );

  const int mask = 16;
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
       std::clog <<   "NET: Reading from SecProc" << std::endl;
     
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
         uint16_t* ptr =  message.u.watch_devices.device_list.device_list;
         uint32_t len =   message.u.watch_devices.device_list.num_devices;
         std::vector<uint16_t> devList( ptr, ptr+len );
         std::clog << "   dev: ";
         for ( const auto& dev : devList ) {  std::clog << dev << " "; }
         std::clog << std::endl;
#if 1
         for ( const auto& dev : devList ) {
           Name name(  message.u.watch_devices.team_id, dev,  message.u.watch_devices.channel_id  );
           std::clog << "   sub=" << name.lname() << std::endl;

           relay.sub( name.sname() , mask );
         }
#endif
        } 
        break;
      case QMsgNetSendASCIIMessage:
        {
        std::clog << "NET: Got SendAsciiMsg from SecProc: "
          << " team=" <<   message.u.send_ascii_message.team_id
          //<< " device=" <<   message.u.send_ascii_message.device_id
          << " ch= " <<  message.u.send_ascii_message.channel_id
          << " val: " << std::string(  (char*)message.u.send_ascii_message.message.data,
                                       message.u.send_ascii_message.message.length )
          << std::endl;

        

#if 1
        Name name( message.u.send_ascii_message.team_id,
                         0x7, // TODO message.u.send_ascii_message.device_id,
                         message.u.send_ascii_message.channel_id,
                         msgNum++ // TODO - persiste msg nums 
                   );
        
          std::clog << "Pub to " << name.lname() << std::endl;
          relay.pub( name.sname(),
                   message.u.send_ascii_message.message.data,
                   message.u.send_ascii_message.message.length );
#else
          secApi.recvAsciiMsg(  message.u.send_ascii_message.team_id,
                                0x7, // TODO message.u.send_ascii_message.device_id,
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

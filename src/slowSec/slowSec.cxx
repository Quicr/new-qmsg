
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <vector>

//#include <cctype>
#include <random>

#include "uiApi.h"
#include "netApi.h"

class NotMLS {
private:
  std::mt19937 psuedoRandGen;
public:
  NotMLS() : psuedoRandGen() {
    std::random_device sysRand;
    psuedoRandGen.seed( sysRand() );
  };

  QMsgDeviceID getRandDevID() {
    std::uniform_int_distribution<QMsgDeviceID> distribution(1, 1*1000*1000) ;
    return distribution(psuedoRandGen)  ;
  };
};

int main( int argc, char* argv[]){

  NotMLS mls;
  
  
  QMsgDeviceID myDeviceID = mls.getRandDevID();
  char *devIdName = getenv( "SLOWR_DEVID" );
  if ( devIdName ) {
    myDeviceID = atoi( devIdName );
  }

  std::clog <<   "SEC: Starting secProc devId=" << std::hex << myDeviceID << std::dec << std::endl;
  assert(   myDeviceID < ( 1<<20 ) );
  
  std::vector<QMsgDeviceID> otherDeviceID;
  otherDeviceID.push_back( myDeviceID );
#if 0
  // TODO - could remove these but for testing with broken subsciptions
  otherDeviceID.push_back( 1 ); 
  otherDeviceID.push_back( 2 );
  otherDeviceID.push_back( 3 );
#endif
  otherDeviceID.push_back( 0 ); // subscribe to whole channel 
  
  UiApi uiApi;
  NetApi netApi;

  while( true ) {
    
    //waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;
    FD_ZERO(&fdSet);
    {
      int net2secFD = netApi.getReadFD();
      FD_SET(net2secFD, &fdSet); maxFD = (net2secFD>maxFD) ? net2secFD : maxFD;
    }
    {
      int ui2secFD= uiApi.getReadFD();
      FD_SET(ui2secFD, &fdSet); maxFD = (ui2secFD>maxFD) ? ui2secFD : maxFD;
    }
    int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
    assert( numSelectFD >= 0 );
    
    // process uiProc
    if ( (numSelectFD > 0) && ( FD_ISSET( uiApi.getReadFD(), &fdSet) ) ) {
      //std::clog <<   "SEC: Reading Sec Proc" << std::endl;
    
      QMsgUIMessage message{};
      uiApi.readMsg( &message );

      switch ( message.type ) {
      case QMsgUIWatchChannel: {
          std::clog << "SEC: Got watch from UIProc:"
                  << " team=" <<    message.u.watch_channel.team_id
                  << " ch=" <<   message.u.watch_channel.channel_id
              << std::endl;

          netApi.watch( message.u.watch_channel.team_id, otherDeviceID,  message.u.watch_channel.channel_id );
      }
        break;
      case QMsgUISendASCIIMessage: {

        std::clog << "SEC: Got AsciiMsg from UIProc: "
          << " team=" <<   message.u.send_ascii_message.team_id
          << " ch=" <<  message.u.send_ascii_message.channel_id
          << " len=" << message.u.send_ascii_message.message.length
          << std::endl;

        std::vector<uint8_t> asciiText( message.u.send_ascii_message.message.data,
                                        message.u.send_ascii_message.message.data + message.u.send_ascii_message.message.length );
        
        netApi.sendAsciiMsg(  message.u.send_ascii_message.team_id,
                              myDeviceID,
                              message.u.send_ascii_message.channel_id,
                              asciiText.data(),
                              asciiText.size() );
                    
      }
        break;
      case QMsgUIInvalid: {
         std::clog << "SEC: Got Invalid msg from UIProc: " << std::endl;
      }
        break;
      default:
        assert(0);
      }
    }
    
    // processs netProc
      if ( (numSelectFD > 0) && ( FD_ISSET(netApi.getReadFD(), &fdSet) ) ) {
        //std::clog <<  "SEC: Reading Net Proc" << std::endl;
        
       QMsgNetMessage message{};
      netApi.readMsg( &message );

      switch ( message.type ) {
    
      case QMsgNetReceiveASCIIMessage:
        {
        std::clog << "SEC: Got AsciiMsg from NetProc: ";
        
         std::clog << " team=" << message.u.receive_ascii_message.team_id
                   << " dev="  << message.u.receive_ascii_message.device_id
                   << " ch="   << message.u.receive_ascii_message.channel_id
                   << " len="  << message.u.receive_ascii_message.message.length
                   << std::endl;

         // get a local copy before memory is invalid
         uint8_t* ptr =  message.u.receive_ascii_message.message.data;
         QMsgLength len =   message.u.receive_ascii_message.message.length;
         std::vector<uint8_t> msg( ptr, ptr+len );

           uiApi.recvAsciiMsg(  message.u.receive_ascii_message.team_id,
                    message.u.receive_ascii_message.device_id,
                    message.u.receive_ascii_message.channel_id,
                                msg.data(),
                                msg.size() );
                    
        }
        break;
      case QMsgNetInvalid:
         std::clog << "Sec: Got Invalid msg from NetProc: " << std::endl;
         break;
      default:
        assert(0);
      }
    }
  }
  
  return 0;
}

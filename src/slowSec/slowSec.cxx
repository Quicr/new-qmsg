
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

#include "uiApi.h"
#include "netApi.h"


int main( int argc, char* argv[]){
  
  std::clog <<   "SEC: Starting secProc" << std::endl;
  
  int myDeviceID = 1; // TODO 
  std::vector<uint16_t> otherDeviceID;
  otherDeviceID.push_back( 1 );
  otherDeviceID.push_back( 2 );
  otherDeviceID.push_back( 7 );
  
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
    
    // processs uiProc
    if ( (numSelectFD > 0) && ( FD_ISSET( uiApi.getReadFD(), &fdSet) ) ) {
       std::clog <<   "SEC: Reading Sec Proc" << std::endl;
    
      QMsgUIMessage message{};
      uiApi.readMsg( &message );

      switch ( message.type ) {
      case QMsgUIWatchChannel:
          std::clog << "SEC: Got watch from UIProc:"
                  << " team=" <<    message.u.watch_channel.team_id
                  << " ch= " <<   message.u.watch_channel.channel_id
              << std::endl;

          netApi.watch( message.u.watch_channel.team_id, otherDeviceID,  message.u.watch_channel.channel_id );
        break;
      case QMsgUISendASCIIMessage:

        std::clog << "SEC: Got AsciiMsg from UIProc: "
          << " team=" <<   message.u.send_ascii_message.team_id
          << " ch= " <<  message.u.send_ascii_message.channel_id
          << " val: " << std::string(  (char*)message.u.send_ascii_message.message.data,
                                       message.u.send_ascii_message.message.length )
          << std::endl;

          netApi.sendAsciiMsg(  message.u.send_ascii_message.team_id,
                    message.u.send_ascii_message.channel_id,
                                myDeviceID,
                    message.u.send_ascii_message.message.data,
                    message.u.send_ascii_message.message.length );
                    
        break;
      case QMsgUIInvalid:
         std::clog << "SEC: Got Invalid msg from UIProc: " << std::endl;
         break;
      default:
        assert(0);
      }
    }
    
    // processs netProc
      if ( (numSelectFD > 0) && ( FD_ISSET(netApi.getReadFD(), &fdSet) ) ) {
        std::clog <<  "SEC: Reading Net Proc" << std::endl;
        
       QMsgNetMessage message{};
      netApi.readMsg( &message );

      switch ( message.type ) {
    
      case QMsgNetReceiveASCIIMessage:
        {
        std::clog << "SEC: Got AsciiMsg from NetProc: ";
        
         std::clog << " team=" <<   message.u.receive_ascii_message.team_id
                  << " device=" <<   message.u.receive_ascii_message.device_id
          << " ch= " <<  message.u.receive_ascii_message.channel_id
          << " val: " << std::string(  (char*)message.u.receive_ascii_message.message.data,
                                       message.u.receive_ascii_message.message.length )
          << std::endl;

         // get a local copy before memory is invalid
         uint8_t* ptr =  message.u.receive_ascii_message.message.data;
         uint32_t len =   message.u.receive_ascii_message.message.length;
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

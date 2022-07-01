#include <assert.h>
#include <iostream>
#include <sys/select.h>

#include "secApi.h"


int main( int argc, char* argv[]){

  std::clog << "UI: Starting\n" << std::endl;
     
  int keyboardFD = 0;
  SecApi secApi;
  
  int team=9;
  int channel= 5;

  secApi.watch( team, channel );

  
  while( true ) {
    //waitForInput
     struct timeval timeout;
     timeout.tv_sec = 1;
     timeout.tv_usec = 0;
     fd_set fdSet;
     int maxFD=0;
     FD_ZERO(&fdSet);
     FD_SET(keyboardFD, &fdSet); maxFD = (keyboardFD>maxFD) ? keyboardFD : maxFD;
     {
       int sec2uiFD = secApi.getReadFD();
       FD_SET(sec2uiFD, &fdSet); maxFD = (sec2uiFD>maxFD) ? sec2uiFD : maxFD;
     }
     int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
     assert( numSelectFD >= 0 );
      
    //processKeyboard
     if ( (numSelectFD > 0) && ( FD_ISSET(keyboardFD, &fdSet) ) ) {
       const int bufSize=128;
       uint8_t keyboardBuf[bufSize];
 
       ssize_t num = read( keyboardFD, keyboardBuf, bufSize );
       if ( num > 0 ) {
         std::clog << "UI: Read %d bytes from keyboard: " << num << std::endl;
         std::string str;
         for ( int i=0; i<num; i++ ) {
           if ( (  keyboardBuf[i] >= 0x20 ) && ( keyboardBuf[i] <= 0x7E ) ) {
             str.push_back( keyboardBuf[i] );
           }
         }
         secApi.sendAsciiMsg( team, channel, (uint8_t*)str.data(), str.size() ); 
       }
     }
     
    //processSecureProc
     if ( (numSelectFD > 0) && ( FD_ISSET(secApi.getReadFD(), &fdSet) ) ) {
       QMsgUIMessage message{};
       secApi.readMsg( &message );
       
       switch ( message.type ) {
       case QMsgUIReceiveASCIIMessage:
         std::clog << "UI: Got AsciiMsg from SecProc: "
                   << " team=" <<   message.u.receive_ascii_message.team_id
                   << " device=" <<   message.u.receive_ascii_message.device_id
                   << " ch= " <<  message.u.receive_ascii_message.channel_id
                   << " val: " << std::string(  (char*)message.u.receive_ascii_message.message.data,
                                                message.u.receive_ascii_message.message.length )
                   << std::endl;
         break;
       case QMsgUIInvalid:
         std::clog << "UI: Got Invalid msg from SecProc: " << std::endl;
         break;
       default:
         assert(0);
       }
      }
  }
  
  return 0;
}

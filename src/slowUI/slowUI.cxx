#include <assert.h>
#include <iostream>
#include <sys/select.h>

#include "secApi.h"


int main( int argc, char* argv[]){

  fprintf(stderr, "UI: Starting\n");
     
  int keyboardFD = 0;
  SecApi secApi;
  
  int team=0x2;
  int channel= 0x3;

  secApi.watch( team, channel );
         
  const int bufSize=128;
  uint8_t secBuf[bufSize];
  uint8_t keyboardBuf[bufSize];
 
  while( true ) {
    //waitForInput
     struct timeval timeout;
     timeout.tv_sec = 1;
     timeout.tv_usec = 0;
     fd_set fdSet;
     int maxFD=0;
     FD_ZERO(&fdSet);
     FD_SET(keyboardFD, &fdSet); maxFD = (keyboardFD>maxFD) ? keyboardFD : maxFD;
     int sec2uiFD = secApi.getReadFD();
     FD_SET(sec2uiFD, &fdSet); maxFD = (sec2uiFD>maxFD) ? sec2uiFD : maxFD;
     int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
     assert( numSelectFD >= 0 );
     //fprintf(stderr, "UI: Running\n");
      
    //processKeyboard
     if ( (numSelectFD > 0) && ( FD_ISSET(keyboardFD, &fdSet) ) ) {
       //fprintf(stderr, "UI: Reading Keyboard\n");
       ssize_t num = read( keyboardFD, keyboardBuf, bufSize );
       if ( num > 0 ) {
         fprintf( stderr, "UI: Read %d bytes from keyboard: ", (int)num );
         fwrite( keyboardBuf, 1 , num , stderr );
         fprintf( stderr, "\n");

         secApi.sendAsciiMsg( team, channel, keyboardBuf, num ); 
       }
     }
     
    //processSecureProc
     if ( (numSelectFD > 0) && ( FD_ISSET(secApi.getReadFD(), &fdSet) ) ) {
       QMsgUIMessage message{};
       secApi.readMsg( &message );
       
       switch ( message.type ) {
       case QMsgUISendASCIIMessage:
         std::clog << "UI: Got AsciiMsg from SecProc: "
                   << " team=" <<   message.u.send_ascii_message.team_id
                   << " ch= " <<  message.u.send_ascii_message.channel_id
                   << " val: " << std::string(  (char*)message.u.send_ascii_message.message.data,
                                                message.u.send_ascii_message.message.length )
                   << std::endl;
         break;
       default:
         assert(0);
       }
      }
  }
  
  return 0;
}

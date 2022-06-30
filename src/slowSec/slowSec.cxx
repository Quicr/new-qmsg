
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "uiApi.h"


int main( int argc, char* argv[]){

  std::clog <<   "SEC: Starting secProc" << std::endl;
     
  int sec2netFD = open( "/tmp/pipe-s2n" , O_WRONLY, O_NONBLOCK );
  assert( sec2netFD >= 0 );
  std::clog <<   "SEC: Got pipe to netProc" << std::endl;

  int net2secFD = open( "/tmp/pipe-n2s" , O_RDONLY, O_NONBLOCK );
  assert( net2secFD >= 0 );
  std::clog <<   "SEC: Got pipe from netProc" << std::endl;
  

  UiApi uiApi;
  
  const int bufSize=128;
  uint8_t netBuf[bufSize];


  QMsgEncoderContext* context = nullptr;
  QMsgEncoderInit( &context );

  
  while( true ) {
    
    //waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;
    FD_ZERO(&fdSet);
    FD_SET(net2secFD, &fdSet); maxFD = (net2secFD>maxFD) ? net2secFD : maxFD;
    int ui2secFD= uiApi.getReadFD();
    FD_SET(ui2secFD, &fdSet); maxFD = (ui2secFD>maxFD) ? ui2secFD : maxFD;
    int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
    assert( numSelectFD >= 0 );
    
    // processs uiProc
    if ( (numSelectFD > 0) && ( FD_ISSET(ui2secFD, &fdSet) ) ) {
       std::clog <<   "SEC: Reading Sec Proc" << std::endl;
    
      QMsgUIMessage message{};
      uiApi.readMsg( &message );

      switch ( message.type ) {
      case QMsgUIWatchChannel:
          std::clog << "SEC: Got watch from UIProc:"
                  << " team=" <<    message.u.watch_channel.team_id
                  << " ch= " <<   message.u.watch_channel.channel_id
              << std::endl;
        break;
      case QMsgUISendASCIIMessage:

        std::clog << "SEC: Got AsciiMsg from UIProc: "
          << " team=" <<   message.u.send_ascii_message.team_id
          << " ch= " <<  message.u.send_ascii_message.channel_id
          << " val: " << std::string(  (char*)message.u.send_ascii_message.message.data,
                                       message.u.send_ascii_message.message.length )
          << std::endl;

          uiApi.sendAsciiMsg(  message.u.send_ascii_message.team_id,
                    message.u.send_ascii_message.channel_id,
                    message.u.send_ascii_message.message.data,
                    message.u.send_ascii_message.message.length );
                    
        break;
      default:
        assert(0);
      }
      
      if (false) { // ( num > 0 ) {
        
        //fprintf( stderr, "SEC: Readfrom UIProc: "  );
        //fwrite( uiBuf, 1 , num , stderr );
        //fprintf( stderr, "\n");
        
        // send to UI processor
        //const char* prefix ="encrypt: ";
        //write( sec2netFD, prefix, strlen( prefix ) );
        //write( sec2netFD, uiBuf, num );
      }
    }
    
    // processs netProc
    if ( (numSelectFD > 0) && ( FD_ISSET(net2secFD, &fdSet) ) ) {
      //fprintf(stderr, "SEC: Reding Sec Proc\n");
      ssize_t num = read( net2secFD, netBuf, bufSize );
      if ( num > 0 ) {
        //fprintf( stderr, "SEC: Read %d bytes from NetProc: ", (int)num );
        //fwrite( netBuf, 1 , num , stderr );
        //fprintf( stderr, "\n");
        
        // send to UI processor
        //const char* prefix ="decrypt: ";
        //write( sec2uiFD, prefix, strlen( prefix ) );
        //write( sec2uiFD, netBuf, num );
      }
    }
  }
  
  return 0;
}

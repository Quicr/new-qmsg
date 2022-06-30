
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "qmsg/encoder.h"


int main( int argc, char* argv[]){

  fprintf(stderr, "UI: Starting\n");
     
  int keyboardFD = 0;

  int sec2uiFD = open( "/tmp/pipe-s2u" , O_RDONLY, O_NONBLOCK );
  assert( sec2uiFD >= 0 );
  fprintf(stderr, "UI: Got pipe from secProc\n");

  int ui2secFD = open( "/tmp/pipe-u2s" , O_WRONLY, O_NONBLOCK );
  assert( ui2secFD >= 0 );
  fprintf(stderr, "UI: Got pipe to secProc\n");

  QMsgEncoderContext* context = nullptr;
  QMsgEncoderInit( &context );
  QMsgUIMessage message{};

  message.type = QMsgUIWatchChannel;
  message.u.watch_channel.team_id = 0x2;
  message.u.watch_channel.channel_id = 0x3;

  char encodeBuffer[1024];
  size_t encodeLen;
  QMsgEncoderResult err;
  err = QMsgUIEncodeMessage( context, &message, encodeBuffer, sizeof(encodeBuffer), &encodeLen ); 
  assert( err == QMsgEncoderSuccess );
  uint32_t sendLen = encodeLen;
  write( ui2secFD, &sendLen, sizeof(sendLen) );
  write( ui2secFD, encodeBuffer, sendLen );
         
  const int bufSize=128;
  char secBuf[bufSize];
  char keyboardBuf[bufSize];
 
  while( true ) {
    //fprintf(stderr, "UI: Loop\n");

    //waitForInput
     struct timeval timeout;
     timeout.tv_sec = 1;
     timeout.tv_usec = 0;
     fd_set fdSet;
     int maxFD=0;
     FD_ZERO(&fdSet);
     FD_SET(keyboardFD, &fdSet); maxFD = (keyboardFD>maxFD) ? keyboardFD : maxFD;
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

         // send to secure processor
         const char* sending ="sending: ";
         write( ui2secFD, sending, strlen( sending ) );
         write( ui2secFD, keyboardBuf, num );
       }
     }
     
    //processSecureProc
     if ( (numSelectFD > 0) && ( FD_ISSET(sec2uiFD, &fdSet) ) ) {
       //fprintf(stderr, "UI: Reding Sec Proc\n");
       ssize_t num = read( sec2uiFD, secBuf, bufSize );
       if ( num > 0 ) {
         fprintf( stderr, "UI: Read %d bytes from SecProc: ", (int)num );
         fwrite( secBuf, 1 , num , stderr );
         fprintf( stderr, "\n");
       }
     }
  }
  
  return 0;
}

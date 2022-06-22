
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>


int main( int argc, char* argv[]){

  fprintf(stderr, "Starting\n");
     
  int keyboardFD = 0;

  int sec2uiFD = open( "/tmp/pipe-s2u" , O_RDONLY, O_NONBLOCK );
  assert( sec2uiFD >= 0 );
  fprintf(stderr, "Got pipe from secProc\n");

  int ui2secFD = open( "/tmp/pipe-u2s" , O_WRONLY, O_NONBLOCK );
  assert( ui2secFD >= 0 );
  fprintf(stderr, "Got pipe to secProc\n");
  
  const int bufSize=128;
  char secBuf[bufSize];
  char keyboardBuf[bufSize];
 
  while( true ) {
    //fprintf(stderr, "Loop\n");

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
     //fprintf(stderr, "Running\n");
      
    //processKeyboard
     if ( (numSelectFD > 0) && ( FD_ISSET(keyboardFD, &fdSet) ) ) {
       //fprintf(stderr, "Reading Keyboard\n");
       ssize_t num = read( keyboardFD, keyboardBuf, bufSize );
       if ( num > 0 ) {
         fprintf( stderr, "Read %d bytes from keyboard: ", (int)num );
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
       //fprintf(stderr, "Reding Sec Proc\n");
       ssize_t num = read( sec2uiFD, secBuf, bufSize );
       if ( num > 0 ) {
         fprintf( stderr, "Read %d bytes from SecProc: ", (int)num );
         fwrite( secBuf, 1 , num , stderr );
         fprintf( stderr, "\n");
       }
     }
  }
  
  return 0;
}

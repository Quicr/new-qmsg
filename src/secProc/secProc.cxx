
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>


int main( int argc, char* argv[]){

  fprintf(stderr, "Starting secProc\n");
     
  int sec2netFD = open( "/tmp/pipe-s2n" , O_WRONLY, O_NONBLOCK );
  assert( sec2netFD >= 0 );
  fprintf(stderr, "Got pipe to netProc\n");

  int net2secFD = open( "/tmp/pipe-n2s" , O_RDONLY, O_NONBLOCK );
  assert( net2secFD >= 0 );
  fprintf(stderr, "Got pipe from netProc\n");
  
  int sec2uiFD = open( "/tmp/pipe-s2u" , O_WRONLY, O_NONBLOCK );
  assert( sec2uiFD >= 0 );
  fprintf(stderr, "Got pipe from uiProc\n");

  int ui2secFD = open( "/tmp/pipe-u2s" , O_RDONLY, O_NONBLOCK );
  assert( ui2secFD >= 0 );
  fprintf(stderr, "Got pipe to uiProc\n");
  
  const int bufSize=128;
  char netBuf[bufSize];
  char uiBuf[bufSize];
 
  while( true ) {
    //fprintf(stderr, "Loop\n");
    
    //waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;
    FD_ZERO(&fdSet);
    FD_SET(net2secFD, &fdSet); maxFD = (net2secFD>maxFD) ? net2secFD : maxFD;
    FD_SET(ui2secFD, &fdSet); maxFD = (ui2secFD>maxFD) ? ui2secFD : maxFD;
    int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
    assert( numSelectFD >= 0 );
    //fprintf(stderr, "Running\n");
    
    // processs uiProc
    if ( (numSelectFD > 0) && ( FD_ISSET(ui2secFD, &fdSet) ) ) {
      //fprintf(stderr, "Reding Sec Proc\n");
      ssize_t num = read( ui2secFD, uiBuf, bufSize );
      if ( num > 0 ) {
        fprintf( stderr, "Read %d bytes from UIProc: ", (int)num );
        fwrite( uiBuf, 1 , num , stderr );
        fprintf( stderr, "\n");
        
        // send to UI processor
        const char* prefix ="encrypt: ";
        write( sec2netFD, prefix, strlen( prefix ) );
        write( sec2netFD, uiBuf, num );
      }
    }
    
    // processs netProc
    if ( (numSelectFD > 0) && ( FD_ISSET(net2secFD, &fdSet) ) ) {
      //fprintf(stderr, "Reding Sec Proc\n");
      ssize_t num = read( net2secFD, netBuf, bufSize );
      if ( num > 0 ) {
        fprintf( stderr, "Read %d bytes from NetProc: ", (int)num );
        fwrite( netBuf, 1 , num , stderr );
        fprintf( stderr, "\n");
        
        // send to UI processor
        const char* prefix ="decrypt: ";
        write( sec2uiFD, prefix, strlen( prefix ) );
        write( sec2uiFD, netBuf, num );
      }
    }
  }
  
  return 0;
}

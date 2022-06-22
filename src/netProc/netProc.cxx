
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>


int main( int argc, char* argv[]){

  fprintf(stderr, "Starting netProc\n");
     
  int sec2netFD = open( "pipe-s2n", O_RDONLY, O_NONBLOCK );
  assert( sec2netFD >= 0 );
  fprintf(stderr, "Got pipe from secProc\n");

  int net2secFD = open( "pipe-n2s", O_WRONLY, O_NONBLOCK );
  assert( net2secFD >= 0 );
  fprintf(stderr, "Got pipe to netProc\n");
  
  const int bufSize=128;
  char secBuf[bufSize];
 
  while( true ) {
    //fprintf(stderr, "Loop\n");
    
    //waitForInput
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fdSet;
    int maxFD=0;
    FD_ZERO(&fdSet);
    FD_SET(sec2netFD, &fdSet); maxFD = (sec2netFD>maxFD) ? sec2netFD : maxFD;
    int numSelectFD = select( maxFD+1 , &fdSet , NULL, NULL, &timeout );
    assert( numSelectFD >= 0 );
    //fprintf(stderr, "Running\n");
    
    // processs secProc
    if ( (sec2netFD > 0) && ( FD_ISSET(sec2netFD, &fdSet) ) ) {
      //fprintf(stderr, "Reding Sec Proc\n");
      ssize_t num = read( sec2netFD, secBuf, bufSize );
      if ( num > 0 ) {
        fprintf( stderr, "Read %d bytes from SecProc: ", (int)num );
        fwrite( secBuf, 1 , num , stderr );
        fprintf( stderr, "\n");
        
        // send to UI processor
        const char* prefix ="reply: ";
        write( net2secFD, prefix, strlen( prefix ) );
        write( net2secFD, secBuf, num );
      }
    }
  }
  
  return 0;
}

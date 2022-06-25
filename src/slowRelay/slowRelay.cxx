
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <slower.h>

int main(int argc, char* argv[]) {
  float slowVer = slowerVersion();

  SlowerConnection slower;
  int err = slowerSetup( slower, slowerDefaultPort  );
  assert( err == 0 );

  while (true ) {
    //std::cerr << "Waiting ... ";
    err=slowerWait( slower );
    assert( err == 0 );
    //std::cerr << "done" << std::endl;
  
    char buf[1200];
    int bufLen=0;
    SlowerRemote remote;
    
    err=slowerRecv( slower, buf, sizeof(buf), &bufLen, &remote );
    assert( err == 0 );
    
    if ( bufLen > 0 ) {
      std::clog << "Got packet of len " << bufLen
                << " from " << inet_ntoa( remote.addr.sin_addr)
                << ":" << remote.addr.sin_port
                << std::endl;

      err = slowerSend( slower, buf, bufLen, remote );
      assert( err == 0 );
    }
  }
  
  return 0;
}


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
  if ( argc != 2 ) {
    float slowVer = slowerVersion();
    std::cerr << "Usage: slowTest <relayName>" << std::endl;
    std::cerr << "slowTest lib version " << slowVer << std::endl;
    exit(-1);
  }
  
  char* relayName =  argv[1];

  SlowerConnection slower;
  int err = slowerSetup( slower  );
  assert( err == 0 );

  SlowerRemote relay;
  err = slowerRemote( relay , relayName );
  if ( err ) {
    std::cerr << "Could not lookup relay IP addres of: " << relayName << std::endl;
  }
  assert( err == 0 );
  err = slowerAddRelay( slower, relay );
  assert( err == 0 );
  
  char msg[] = "Hello";
  ShortName name;
  name.part[0] = 1; name.part[1] = 2;
  //err = slowerSend( slower, msg , strlen(msg) , relay );
  err = slowerPub( slower,  name,  msg , strlen(msg)  );
  assert( err == 0 );

  while ( true ) {
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
    }
  }

  slowerClose( slower );
                       
  return 0;
}

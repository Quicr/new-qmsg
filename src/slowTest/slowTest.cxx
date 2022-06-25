
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

  char msg[] = "Hello";
  err = slowerSend( slower, msg , strlen(msg) , relay );
  assert( err == 0 );
   
  return 0;
}

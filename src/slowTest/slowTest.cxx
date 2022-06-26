
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
  if ( ( argc != 3 ) && (  argc != 4 ) ) {
    float slowVer = slowerVersion();
    std::cerr << "Usage PUB: slowTest <relayName> 1234 pubData" << std::endl; 
    std::cerr << "Usage SUB: slowTest <relayName> 1234 " << std::endl;
    std::cerr << "slowTest lib version " << slowVer << std::endl;
    exit(-1);
  }
  
  char* relayName =  argv[1];
  char* name =  argv[2];
  int mask=16;
  char* data = NULL;
  if ( argc == 4 ) {
    data = argv[3];
  }

  SlowerConnection slower;
  int err = slowerSetup( slower  );
  assert( err == 0 );

  SlowerRemote relay;
  err = slowerRemote( relay , relayName );
  if ( err ) {
    std::cerr << "Could not lookup relay IP addres of: " << relayName << std::endl;
  }
  assert( err == 0 );
  std::clog << "Using relay at " << inet_ntoa( relay.addr.sin_addr)  << ":" <<  ntohs(relay.addr.sin_port) << std::endl;
   
  err = slowerAddRelay( slower, relay );
  assert( err == 0 );
  
  ShortName shortName;
  shortName.part[0] = 1; shortName.part[1] = 2;

  if ( data ) {
    // do publish 
    err = slowerPub( slower,  shortName,  data , strlen(data)  );
    assert( err == 0 );

     while ( true ) {
      err=slowerWait( slower );
      assert( err == 0 );

      ShortName recvName;
      err = slowerRecvAck( slower, &recvName );
      assert( err == 0 );
      if ( recvName == shortName ) {
        std::clog << "Got ACK for data" << std::endl;
        break;
      }
     }
  }
  else {
    // do subscribe 
    err = slowerSub( slower,  shortName, mask  );
    assert( err == 0 );

    while ( true ) {
      err=slowerWait( slower );
      assert( err == 0 );
      
      
      char buf[slowerMTU];
      int bufLen=0;
      ShortName recvName;
      
      err = slowerRecvPub( slower, &recvName, buf, sizeof(buf), &bufLen );
      assert( err == 0 );
      if ( bufLen > 0 ) {
        std::clog << "Got packet of len " << bufLen << " : " ;
        for ( int i=0; i< bufLen; i++ ) {
          char c = buf[i];
          if (( c >= 32 ) && ( c <= 0x7e ) ) {
            std::clog << c;
          }
        }
        std::clog << std::endl;
        // break;
      }
    }
    
    err = slowerUnSub( slower, shortName, mask );
    assert( err == 0 );
  }
    
  slowerClose( slower );
  return 0;
}

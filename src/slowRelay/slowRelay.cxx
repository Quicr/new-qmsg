
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
  
    char buf[slowerMTU];
    int bufLen=0;
    SlowerRemote remote;
    ShortName name;
    SlowerMsgType type;
    int mask;
    
    err=slowerRecvMulti(  slower, &name, &type, &remote, &mask,  buf, sizeof(buf), &bufLen );
    assert( err == 0 );
    
    if ( ( type == SlowerMsgPub ) && ( bufLen > 0 ) ) {
      std::clog << "Got PUB of len " << bufLen
                << " from " << inet_ntoa( remote.addr.sin_addr)
                << ":" << remote.addr.sin_port
                << std::endl;

      err = slowerPub( slower, name, buf, bufLen, &remote );
      assert( err == 0 );
    }

    if ( type == SlowerMsgSub  ) {
      std::clog << "Got SUB from " << inet_ntoa( remote.addr.sin_addr)
                << ":" << remote.addr.sin_port
                << std::endl;
    }
       
    if ( type == SlowerMsgUnSub  ) {
      std::clog << "Got UnSUB from " << inet_ntoa( remote.addr.sin_addr)
                << ":" << remote.addr.sin_port
                << std::endl;
    }
    
     
  }

  slowerClose( slower );
  return 0;
}

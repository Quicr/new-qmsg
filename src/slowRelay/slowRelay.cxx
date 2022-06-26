
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

#include <list>

#include <slower.h>

int main(int argc, char* argv[]) {
  float slowVer = slowerVersion();

  SlowerConnection slower;
  int err = slowerSetup( slower, slowerDefaultPort  );
  assert( err == 0 );

  std::list<SlowerRemote> subscribeList;

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
                << ":" << ntohs( remote.addr.sin_port )
                << std::endl;

      err = slowerAck( slower, name, &remote );
      assert( err == 0 );

      for ( SlowerRemote dest:subscribeList ) {
        if ( dest != remote ) {
           std::clog << "  Sent to " << inet_ntoa( dest.addr.sin_addr) << ":" << ntohs( dest.addr.sin_port ) << std::endl;
           err = slowerPub( slower, name, buf, bufLen, &dest );
        }
      }
    }

    if ( type == SlowerMsgSub  ) {
      std::clog << "Got SUB from " << inet_ntoa( remote.addr.sin_addr)
                << ":" <<  ntohs( remote.addr.sin_port )
                << " mask=" << mask 
                << std::endl;
      subscribeList.push_back( remote );
    }
       
    if ( type == SlowerMsgUnSub  ) {
      std::clog << "Got UnSUB from " << inet_ntoa( remote.addr.sin_addr)
                << ":" <<  ntohs( remote.addr.sin_port )
                << std::endl;
    }
    
     
  }

  slowerClose( slower );
  return 0;
}

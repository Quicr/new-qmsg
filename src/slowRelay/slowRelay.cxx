
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

#include "Subscriptions.h"


class Cache {
public:
  void put( const ShortName& name,  std::vector<uint8_t>& data );
  std::vector<uint8_t> get( const ShortName& name );
  bool exists(  const ShortName& name );
  std::list<ShortName> find(  const ShortName& name, const int mask );
private:
  std::map< ShortName, std::vector<uint8_t> > dataCache;
};
  

int main(int argc, char* argv[]) {
  float slowVer = slowerVersion();

  SlowerConnection slower;
  int err = slowerSetup( slower, slowerDefaultPort  );
  assert( err == 0 );

  Subscriptions subscribeList;

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

 
      std::list<SlowerRemote> list =  subscribeList.find( name );
      for ( SlowerRemote dest: list ) {
        if ( dest != remote ) {
           std::clog << "  Sent to sub " << inet_ntoa( dest.addr.sin_addr) << ":" << ntohs( dest.addr.sin_port ) << std::endl;
           err = slowerPub( slower, name, buf, bufLen, &dest );
        }
      }
    }

    if ( type == SlowerMsgSub  ) {
      std::clog << "Got SUB from " << inet_ntoa( remote.addr.sin_addr)
                << ":" <<  ntohs( remote.addr.sin_port )
                << " mask=" << mask 
                << std::endl;
      subscribeList.add( name, mask, remote );
    }
       
    if ( type == SlowerMsgUnSub  ) {
      std::clog << "Got UnSUB from " << inet_ntoa( remote.addr.sin_addr)
                << ":" <<  ntohs( remote.addr.sin_port )
                << std::endl;
       subscribeList.remove( name, mask, remote );
    }
    
     
  }

  slowerClose( slower );
  return 0;
}

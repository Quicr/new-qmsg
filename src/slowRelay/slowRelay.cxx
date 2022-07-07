
#include <arpa/inet.h>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <slower.h>
#include <name.h>

#include "subscription.h"
#include "cache.h"


int main(int argc, char* argv[]) {
  std::clog << "Starting slowerReal (slower version " << slowerVersion() << ")" << std::endl;
  SlowerConnection slower;
  int err = slowerSetup( slower, slowerDefaultPort  );
  assert( err == 0 );

  
  // ========  Setup up upstream and relay mesh =========
  std::list<SlowerRemote>  relays;
  for ( int i=1; i< argc; i++ ) {
     SlowerRemote relay;
     err = slowerRemote( relay , argv[i] );
     if ( err ) {
       std::cerr << "Could not lookup IP address for relay: " << argv[i]  << std::endl;
     } else {
       std::clog << "Using relay at " << inet_ntoa( relay.addr.sin_addr)  << ":" <<  ntohs(relay.addr.sin_port) << std::endl;
       relays.push_back( relay );
     }
  }
  // get relays from ENV var
  char* relayEnv = getenv( "SLOWER_RELAYS" );
  if ( relayEnv ) {
    std::string rs( relayEnv );
    std::stringstream ss( rs );
    std::vector<std::string> relayNames;

    std::string buf;
    while ( ss >> buf ) {
      relayNames.push_back( buf );
    }
    
    for ( auto r : relayNames ) {
      SlowerRemote relay;
      err = slowerRemote( relay , (char*)r.c_str() );
      if ( err ) {
        std::cerr << "Could not lookup IP address for relay: " << r  << std::endl;
      } else {
        std::clog << "Using relay at " << inet_ntoa( relay.addr.sin_addr)  << ":" <<  ntohs(relay.addr.sin_port) << std::endl;
        relays.push_back( relay );
      }
    }
  }
  

  // ========== MAIN Loop ==============
  Subscriptions subscribeList;
  Cache cache;

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

    // =========  PUBLISH ===================
    if ( ( type == SlowerMsgPub ) && ( bufLen > 0 ) ) {
      bool duplicate = cache.exists( name );
      
      std::clog << "Got "
                << ( duplicate ? "dup " : "" )
                << "PUB " << Name(name).longString()
                << " from " << inet_ntoa( remote.addr.sin_addr)
                << ":" << ntohs( remote.addr.sin_port )
                << std::endl;

      err = slowerAck( slower, name, &remote );
      assert( err == 0 );
        
      if ( !duplicate ) {
        // add to local cache 
        std::vector<uint8_t> data( buf, buf+bufLen );
        cache.put( name,  data );

        // report metrics for QMsg
        if ( true ) {
          Name qmsgName( name );
          if ( ( qmsgName.orginID() == 0x88 )
               && ( qmsgName.appID() == 0x88 )
               && ( qmsgName.path() == NamePath::message )
               && ( data.size() >= 6 ) ) {
            uint64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

            uint64_t thenMs = 0;
            for ( int i=0; i<6; i++ ) {
              thenMs <<= 8;
              thenMs |= data[i];
            }

            std::clog << "    " << qmsgName.longString() << " latency:" << ( nowMs - thenMs ) << std::endl;
          }
        }
        
        // send to other relays
        for ( SlowerRemote dest: relays ) {
            if ( dest != remote ) {
            std::clog << "  Sent to relay " << inet_ntoa( dest.addr.sin_addr) << ":" << ntohs( dest.addr.sin_port ) << std::endl;
            err = slowerPub( slower, name, buf, bufLen, &dest );
            assert( err == 0 );
          }
        }
         
        // send to anyone subscribed
        std::list<SlowerRemote> list =  subscribeList.find( name );
        for ( SlowerRemote dest: list ) {
          if ( dest != remote ) {
            std::clog << "  Sent to subscriber " << inet_ntoa( dest.addr.sin_addr) << ":" << ntohs( dest.addr.sin_port ) << std::endl;
            err = slowerPub( slower, name, buf, bufLen, &dest );
            assert( err == 0 );
          }
        }
      }
    }

    // ============ SUBSCRIBE ==================
    if ( type == SlowerMsgSub  ) {
      std::clog << "Got SUB" 
                << " for " << Name(name).longString() << "*" << mask
                << " from " << inet_ntoa( remote.addr.sin_addr) << ":" <<  ntohs( remote.addr.sin_port )
                << std::endl;
      subscribeList.add( name, mask, remote );
      
      std::list<ShortName> names = cache.find( name, mask );
      names.reverse(); // send the highest (and likely most recent) first 

      for ( auto n : names ) {
        std::clog << "  Sent cache " << Name(n).longString() << std::dec << std::endl;
        const std::vector<uint8_t>* priorData = cache.get( n );

        if ( priorData->size() != 0 ) {
          err = slowerPub( slower, n, (char*)(priorData->data()), priorData->size(), &remote );
          assert( err == 0 );
        } else {
             std::clog << "    PROBLEM with missing priorData" << std::endl;
        } 
      }
  
    }

    // ============== Un SUBSSCRIBE ===========
    if ( type == SlowerMsgUnSub  ) {
       std::clog << "Got UnSUB" 
                 << " for " <<  Name(name).longString() << "*" << mask
                 << " from " << inet_ntoa( remote.addr.sin_addr) << ":" <<  ntohs( remote.addr.sin_port )
                 << std::endl;
       subscribeList.remove( name, mask, remote );
    }  
  }

  slowerClose( slower );
  return 0;
}

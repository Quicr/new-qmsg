
#include <algorithm>
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
#include <vector>
#include <iomanip>

#include <chrono>


#include <slower.h>
#include <name.h>


int main(int argc, char* argv[]) {
  if ( ( argc != 2 ) && (  argc != 3 ) ) {
    float slowVer = slowerVersion();
    std::cerr << "Relay address and port set in SLOWR_RELAY and SLOWR_PORT env variables as well as SLOWR_ORG" << std::endl; 
    std::cerr << "Usage PUB: slowTest <team>/<channel>/<device/<message> pubData" << std::endl; 
    std::cerr << "Usage SUB: slowTest <team>/<channel>/<device/<message>" << std::endl; 
    std::cerr << "Usage SUB: slowTest <team>/<channel>/<device>" << std::endl;
    std::cerr << "Usage SUB: slowTest <team>/<channel>" << std::endl;
    std::cerr <<  std::endl; 
    std::cerr << "Usage PUB: slowTest FF0001 pubData" << std::endl; 
    std::cerr << "Usage SUB: slowTest FF0000:16" << std::endl;
    std::cerr << "slowTest lib version " << slowVer << std::endl;
    exit(-1);
  }
  
  char* relayName =  getenv( "SLOWR_RELAY" );
  if ( !relayName ) {
    static  char defaultRelay[]  = "relay.us-east-2.qmsg.ctgpoc.com ";
    relayName = defaultRelay;
  }
   
  uint32_t org = 1;
  char* orgVar = getenv( "SLOWR_ORG" ); 
  if ( orgVar ) {
    org = atoi( orgVar );
  }

  int port = 5004;
  char* portVar = getenv( "SLOWR_PORT" ); 
  if ( portVar ) {
    port = atoi( portVar );
  }

  std::string nameString( argv[1] );
  MsgShortName shortName;
  int mask=16;
  try {
    if ( nameString.find('/') != std::string::npos ) {
      uint32_t team=1;
      uint32_t channel=1;
      uint32_t device=0;
      uint32_t msgID=0;

      team    = std::stoul( nameString.substr(0,  nameString.find('/') ), nullptr, 16);
      nameString.erase( 0, nameString.find('/') );

      channel = std::stoul( nameString.substr(1,  nameString.find('/',1) ), nullptr, 16);
      nameString.erase( 0, nameString.find('/',1) );
      mask=40;

      if ( nameString.length() > 0 ) {
        device  = std::stoul( nameString.substr(1,  nameString.find('/',1) ), nullptr, 16);
        nameString.erase( 0, nameString.find('/',1) );
        mask = 20;
        
        if ( nameString.length() > 0 ) {
          msgID   = std::stoul( nameString.substr(1,  nameString.length() ), nullptr, 16);
          nameString.erase( 0, nameString.length() );
          mask = 0; 
        }
      }
      
      Name nameObj( NamePath::message, org, team, channel, device , msgID );
      shortName = nameObj.shortName();
    }

    /* --- Add support for hex names if that's going to be used.
    else {
      if ( nameString.find(':') != std::string::npos ) {
        mask = std::stoul( nameString.substr( nameString.find(':')+1, std::string::npos ), nullptr, 10);
        nameString.erase( nameString.find(':'), std::string::npos );
      }
      std::string low;
      std::string high;
      if ( nameString.length() <= 16 ) {
        low = nameString;
        shortName.part[0] = std::stoul(low,nullptr,16);
        shortName.part[1] = 0;
      } else {
        low =  nameString.substr( nameString.length()-16 , std::string::npos );
        high = nameString.substr( 0 , nameString.length()-16 );
        shortName.part[0] = std::stoul(low,nullptr,16);
        shortName.part[1] = std::stoul(high,nullptr,16);
      }
    } */
  } catch ( ... ) {
    std::clog << "invalid input name: " << nameString <<  std::endl;
    exit (1);
  }
 
  std::cerr << "Name=" << Name( shortName ).shortString()
            << " " <<  Name( shortName ).longString() << std::endl; 
  
    
  std::vector<uint8_t> data;
  if ( argc == 3 ) {
    data.resize(6);
    uint64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
    uint64_t d=nowMs;
    for ( int i=5; i >= 0; --i ) {
      data[i] = ( (uint8_t)( d & 0xFF)  );
      d >>= 8;
    }
         
    data.insert( data.end(), (uint8_t*)(argv[2]) , ((uint8_t*)(argv[2])) + strlen( argv[2] ) );
  }

  
  SlowerConnection slower;
  int err = slowerSetup( slower  );
  assert( err == 0 );
  
  SlowerRemote relay;
  err = slowerRemote( relay , relayName, port );
  if ( err ) {
    std::cerr << "Could not lookup IP address for relay: " << relayName << std::endl;
  }
  assert( err == 0 );
  std::clog << "Using relay at " << inet_ntoa( relay.addr.sin_addr)  << ":" <<  ntohs(relay.addr.sin_port) << std::endl;

  
  err = slowerAddRelay( slower, relay );
  assert( err == 0 );

  
  if ( data.size() > 0  ) {
    // do publish
    std::clog << "PUB to "
              <<  Name( shortName ).shortString()
              << " aka "
              << Name( shortName ).longString()
              << std::endl;
    err = slowerPub( slower,  shortName,  (char*)data.data() , data.size()  );
    assert( err == 0 );

     while ( true ) {
      err=slowerWait( slower );
      assert( err == 0 );

      MsgShortName recvName;
      err = slowerRecvAck( slower, &recvName );
      assert( err == 0 );
      if ( recvName == shortName ) {
        std::clog << "   Got ACK for " << Name( recvName ).longString() << std::endl;
        break;
      }
     }
  }
  else {
    // do subscribe 
    std::clog << "SUB to "
              <<  Name( shortName ).shortString() << ":" << mask
              << " aka "
              << Name( shortName ).longString()   << ":" << mask
              << std::endl;

    assert( mask < 64 );
    
    err = slowerSub( slower,  shortName, mask  );
    assert( err == 0 );

    while ( true ) {
      err=slowerWait( slower );
      assert( err == 0 );

      char buf[slowerMTU];
      int bufLen=0;
      MsgShortName recvName;
      
      err = slowerRecvPub( slower, &recvName, buf, sizeof(buf), &bufLen );
      assert( err == 0 );
      if ( bufLen > 0 ) {
        
        uint64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();

        uint64_t thenMs = 0;
        if ( bufLen > 6 ) {
          for ( int i=0; i<6; i++ ) {
            thenMs <<= 8;
            thenMs |= (uint8_t)(buf[i]);
          }
        }

        std::chrono::milliseconds thenDuration( thenMs ); //  thenMs );
        std::chrono::time_point<std::chrono::system_clock> thenTimePoint( thenDuration );
        std::time_t thenCTime = std::chrono::system_clock::to_time_t( thenTimePoint );
        std::tm thenDateTime = *std::localtime( &thenCTime );
           
        std::clog << "Got data for "
                  << Name( recvName ).longString()
          //<< " len=" << bufLen 
                  << " at " << std::put_time(&thenDateTime, "%T")
                  << " ";
        for ( int i=6; i< bufLen; i++ ) {
          char c = buf[i];
          if (( c >= 32 ) && ( c <= 0x7e ) ) {
            std::clog << c;
          }
          else {
             std::clog << '~';
          }
        }
        std::clog  << " latency:" << ( nowMs - thenMs )<< std::endl;
        // break;
      }
    }
    
    err = slowerUnSub( slower, shortName, mask );
    assert( err == 0 );
  }
    
  slowerClose( slower );
  return 0;
}

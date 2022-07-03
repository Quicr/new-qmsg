
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __linux__
#include <net/ethernet.h>
#include <netpacket/packet.h>
#else
#include <net/if_dl.h>
#endif

#include <slower.h>

static int slowerRecv( SlowerConnection& slower, char buf[], int bufSize, int* bufLen, SlowerRemote* remote );
static int slowerSend( SlowerConnection& slower, char buf[], int bufLen, SlowerRemote& remote );

float slowerVersion() {
  return 0.11;
}


bool operator!=( const SlowerRemote& a, const SlowerRemote& b ){
  if ( a.addr.sin_port != b.addr.sin_port ) return true;
  if ( a.addrLen != b.addrLen ) return true;
  if ( memcmp( &(a.addr.sin_addr), &(b.addr.sin_addr),4 ) != 0)  return true; // TODO broken for v6
  return false;
}

bool operator<( const SlowerRemote& a, const SlowerRemote& b ){
  if ( a.addr.sin_port > b.addr.sin_port ) return true;
  if ( a.addr.sin_port < b.addr.sin_port ) return false;
  if ( a.addrLen > b.addrLen ) return true;
  if ( a.addrLen < b.addrLen ) return false;
  if ( memcmp( &(a.addr.sin_addr), &(b.addr.sin_addr),4 ) > 0)  return true; // TODO broken for v6
  return false;
}


bool operator<( const ShortName& a, const ShortName& b )
{
  if (a.part[1] > b.part[1] ) return false;
  if (a.part[1] < b.part[1] ) return true;
  return (a.part[0] < b.part[0] );
}

bool operator==( const ShortName& a, const ShortName& b ){
  return ( (a.part[0] == b.part[0]) && (a.part[1] == b.part[1] ) );
}

bool operator!=( const ShortName& a, const ShortName& b ){
  return ( (a.part[0] != b.part[0]) || (a.part[1] != b.part[1] ) );
}


int slowerSetup( SlowerConnection& slower, uint16_t port) {
  slower.fd=0;
  bzero( &slower.relay, sizeof( slower.relay ) );
  
  int err;
  
  slower.fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (slower.fd < 0) {
    perror("probelm opening socket");
    exit(-1);
  }

  int optionValue = 1;
  err = setsockopt(slower.fd, SOL_SOCKET, SO_REUSEPORT, (const void *)&optionValue,
                   sizeof(optionValue));
  if (err != 0) {
    perror("problem setting socket re-use option");
    exit(-2);
  }

  if ( port != 0 ) {
    struct sockaddr_in srvAddr;
    bzero((char *)&srvAddr, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(port);
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    err = bind(slower.fd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    if (err < 0) {
      perror("problem on binding to port to listen on");
      exit(-3);
    }
  }

  struct timeval timeOut;
  timeOut.tv_sec = 0;
  timeOut.tv_usec = 10 * 1000;
  int serr = setsockopt( slower.fd, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(timeOut));
  if (serr) {
    std::cerr << "Problem setting timeout on socket" << std::endl;
  }
  
  return 0;
}

int slowerRemote(  SlowerRemote& remote, const char* server, const uint16_t port ){
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED | AI_ALL;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  // Get IP address of remote 
  std::string portString = std::to_string(port);
  struct addrinfo* results=NULL;
  int err = getaddrinfo(server, portString.c_str() , &hints, &results);
  if (err) {
    perror("Failed to lookup remote host");
    return -1; 
  }

  struct addrinfo* result=NULL;
  for ( result = results; result != NULL; result = result->ai_next) {
    if (result->ai_addrlen > sizeof(remote.addr)) continue;
    
    memset(&remote.addr, 0, sizeof(remote.addr));
    assert(result->ai_addrlen <= sizeof(remote.addr));

    remote.addrLen = results->ai_addrlen;
    memcpy(&remote.addr, results->ai_addr, result->ai_addrlen);
#if 0
    std::clog << "Got remote IP of " << inet_ntoa( remote.addr.sin_addr) << std::endl;
#endif
    break; // found a good result 
  }
  if ( result == NULL ) {
    // no result found
     freeaddrinfo(results);
     return -1;
  }
  
  freeaddrinfo(results);
  return 0;
}

static int slowerSend( SlowerConnection& slower, char buf[], int bufLen, SlowerRemote* remote ) {
  assert( bufLen <= 1200 );
  assert( slower.fd > 0 );

  SlowerRemote dest;
  if ( remote == NULL ) {
    dest = slower.relay;
  } else {
    dest = *remote;
  }

#if 0
  std::clog << "in slowerSend with"
            << " bufLen=" << bufLen
            << " IP=" << inet_ntoa( dest.addr.sin_addr)
            << " port=" << ntohs( dest.addr.sin_port )
            << " remote=" << ( (remote)?1:0 ) 
            << std::endl;
#endif
    
  int n = sendto( slower.fd, buf, bufLen, 0 /*flags*/,
                  (struct sockaddr*)&dest.addr, dest.addrLen);
  if (n < 0) {
    perror("UDP sendto error");
    return -1;
  } else if (n != bufLen) {
    perror("UDP sendto failed");
    return -1;
  }
  
  return 0;
}



int slowerGetFD( SlowerConnection& slower) {
  return slower.fd;
}

int slowerWait( SlowerConnection& slower ){
  fd_set fdSet;
  FD_ZERO( &fdSet );
  int maxFd=0;

  FD_SET( slower.fd , &fdSet ); maxFd = (slower.fd>maxFd) ? maxFd = slower.fd : maxFd;

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 500*1000;
     
  int err = select( maxFd+1, &fdSet, NULL, NULL, &timeout );
  if ( err < 0 ) {
    perror("Error on select");
    return -1;
  }
  
  return 0;
}

static int slowerRecv( SlowerConnection& slower, char buf[], int bufSize, int* bufLen, SlowerRemote* remote ) {
  assert( slower.fd > 0 );

  bzero( buf, bufSize );

  bzero( &(remote->addr) , sizeof(  remote->addr ) );
  remote->addrLen = sizeof(  remote->addr );
  *bufLen = 0; 


  //ssize_t r = recv(slower.fd, (void*)buf, (size_t)bufSize, 0 /*flags*/) ;
  ssize_t r = recvfrom(slower.fd, (void*)buf, (size_t)bufSize, 0 /*flags*/,
                       (struct sockaddr *)&(remote->addr),
                       &(remote->addrLen)
                       );
  if ( r == -1 ) {
    int e = errno ;
    if ( e == EAGAIN ) {
      return 0;
    }
    std::cerr << "revc udp packet got error: " << strerror(e) << std::endl;
     return -1;
   }
   *bufLen = r ;

#if 0
   std::clog << "in slowerRecv with bufLen=" << *bufLen << std::endl;
#endif
   
   return 0;
}


int slowerAddRelay( SlowerConnection& slower, const SlowerRemote& remote ){
  slower.relay = remote;
  return 0;
}

  
int slowerPub( SlowerConnection& slower, const ShortName& name, char buf[], int bufLen, SlowerRemote* remote ){
  assert( bufLen < slowerMTU-20 ); 
  assert( slower.fd > 0 );
  assert( bufLen > 0 );
  
  char msg[slowerMTU];
  int msgLen=0;

  int8_t type = SlowerMsgPub;
  memcpy( msg+msgLen, &type, sizeof(type) ) ; msgLen += sizeof(type);

  memcpy( msg+msgLen, &name.part[0], sizeof(name.part[0]) ) ; msgLen += sizeof(name.part[0]);
  memcpy( msg+msgLen, &name.part[1], sizeof(name.part[1]) ) ; msgLen += sizeof(name.part[1]);

  int16_t dataLen =  bufLen;
  memcpy( msg+msgLen, &dataLen, sizeof(dataLen) ) ; msgLen += sizeof(dataLen);

  assert( msgLen + bufLen < sizeof(msg) );
  memcpy( msg+msgLen, buf, bufLen ) ; msgLen += bufLen;
  assert( msgLen < sizeof( msg ) );
  
  int err = slowerSend( slower, msg, msgLen, remote );
  return err;
}


int slowerRecvMulti( SlowerConnection& slower, ShortName* name, SlowerMsgType* msgType,  SlowerRemote* remote, int* mask, char buf[], int bufSize, int* bufLen ){
  assert( name );
  assert( msgType );
  assert( remote );
  assert( mask );
  assert( buf );
  assert( bufLen );
  assert( bufSize > 0 );

  *msgType = SlowerMsgInvalid;
  *mask=0;
  *bufLen=0;
  bzero( name, sizeof( *name ) );
   
  char msg[slowerMTU];
  int msgLen=0; // total length of data received 
  int msgLoc=0; // position of current decode of messages
  
  int err = slowerRecv( slower, msg, sizeof(msg), &msgLen, remote );
  if ( err != 0 ) {
    return err;
  }
  if ( msgLen == 0 ) {
    return 0;
  }
  
  int8_t type;
  assert( msgLen >= msgLoc + sizeof( type ) ); memcpy( &type, msg+msgLoc, sizeof(type) ); msgLoc += sizeof(type);
  *msgType = (SlowerMsgType)type;
  assert( *msgType != SlowerMsgInvalid );

  assert( msgLen >= msgLoc + sizeof( name->part[0] ) ); memcpy( &(name->part[0]), msg+msgLoc, sizeof(name->part[0]) ); msgLoc += sizeof(name->part[0]);
  assert( msgLen >= msgLoc + sizeof( name->part[1] ) ); memcpy( &(name->part[1]), msg+msgLoc, sizeof(name->part[1]) ); msgLoc += sizeof(name->part[1]);

  switch (*msgType) {
  case SlowerMsgPub:
    int16_t dataLen;
    assert( msgLen > msgLoc + sizeof( dataLen ) ); memcpy( &dataLen, msg+msgLoc, sizeof(dataLen) ); msgLoc += sizeof(dataLen);

    assert( dataLen < bufSize );
    assert( msgLen >= msgLoc + dataLen ); memcpy( buf, msg+msgLoc, dataLen ); msgLoc += dataLen;
    *bufLen = dataLen;

    assert( msgLoc == msgLen );
    break;
  case SlowerMsgSub:
    int8_t maskData;
    assert( msgLen >= msgLoc + sizeof( maskData ) ); memcpy( &maskData, msg+msgLoc, sizeof(maskData) ); msgLoc += sizeof(maskData);
    *mask = maskData;
    assert( msgLoc == msgLen );
    break;
  case SlowerMsgUnSub:
    assert( msgLoc == msgLen );
    break;
  case SlowerMsgAck:
    assert( msgLoc == msgLen );
    break;
  default:
    return -1;
  }

  return 0;
}

int slowerRecvAck( SlowerConnection& slower, ShortName* name ){
  assert( name );
  
  SlowerMsgType type;
  SlowerRemote remote;
  int mask;

  char buf[slowerMTU];
  int bufSize=sizeof(buf);
  int bufLen=0;
    
  int err = slowerRecvMulti( slower,name, &type, &remote, &mask, buf, bufSize, &bufLen );
  if ( err != 0 ) {
    return err;
  }
  if ( type != SlowerMsgAck ) {
    bzero( name, sizeof( *name ) );
    return 0;
  }
  
  return 0;
}


int slowerRecvPub( SlowerConnection& slower, ShortName* name, char buf[], int bufSize, int* bufLen ){
  assert( name );
  assert( bufLen );
  assert( bufSize > 0 );
  
  SlowerMsgType type;
  SlowerRemote remote;
  int mask;
  
  int err = slowerRecvMulti( slower,name, &type, &remote, &mask, buf, bufSize, bufLen );
  if ( err != 0 ) {
    return err;
  }
  if ( type != SlowerMsgPub ) {
    bzero( name, sizeof( *name ) );
    *bufLen = 0;
    return 0;
  }
  
  return 0;
}


int slowerAck( SlowerConnection& slower, const ShortName& name,  SlowerRemote* remote ){
  assert( slower.fd > 0 );
  
  char msg[slowerMTU];
  int msgLen=0;

  int8_t type = SlowerMsgAck;
  memcpy( msg+msgLen, &type, sizeof(type) ) ; msgLen += sizeof(type);

  memcpy( msg+msgLen, &name.part[0], sizeof(name.part[0]) ) ; msgLen += sizeof(name.part[0]);
  memcpy( msg+msgLen, &name.part[1], sizeof(name.part[1]) ) ; msgLen += sizeof(name.part[1]);

  assert( msgLen < sizeof( msg ) );
          
  int err = slowerSend( slower, msg, msgLen, remote );
  return err;
}



int slowerSub( SlowerConnection& slower, const ShortName& name, int mask , SlowerRemote* remote ){
  assert( slower.fd > 0 );
  assert( mask >= 0 );
  assert( mask < 128 ); 
  
  char msg[slowerMTU];
  int msgLen=0;

  int8_t type = SlowerMsgSub;
  memcpy( msg+msgLen, &type, sizeof(type) ) ; msgLen += sizeof(type);

  memcpy( msg+msgLen, &name.part[0], sizeof(name.part[0]) ) ; msgLen += sizeof(name.part[0]);
  memcpy( msg+msgLen, &name.part[1], sizeof(name.part[1]) ) ; msgLen += sizeof(name.part[1]);

  int8_t maskData = mask;
  memcpy( msg+msgLen, &maskData, sizeof(maskData) ) ; msgLen += sizeof(maskData);

  assert( msgLen < sizeof( msg ) );
          
  int err = slowerSend( slower, msg, msgLen, remote );
  return err;
}


int slowerUnSub( SlowerConnection& slower, const ShortName& name, int mask , SlowerRemote* remote  ) {
  assert( slower.fd > 0 );
  assert( mask >= 0 );
  assert( mask < 128 ); 
  
  char msg[slowerMTU];
  int msgLen=0;

  int8_t type = SlowerMsgUnSub;
  memcpy( msg+msgLen, &type, sizeof(type) ) ; msgLen += sizeof(type);

  memcpy( msg+msgLen, &name.part[0], sizeof(name.part[0]) ) ; msgLen += sizeof(name.part[0]);
  memcpy( msg+msgLen, &name.part[1], sizeof(name.part[1]) ) ; msgLen += sizeof(name.part[1]);

  int8_t maskData = mask;
  memcpy( msg+msgLen, &maskData, sizeof(maskData) ) ; msgLen += sizeof(maskData);

  assert( msgLen < sizeof( msg ) );
          
  int err = slowerSend( slower, msg, msgLen, remote );
  return err;
}


int slowerClose( SlowerConnection& slower ){
  close( slower.fd ); slower.fd=0;
  bzero( & slower.relay, sizeof( slower.relay ) );
  
  return 0;
}

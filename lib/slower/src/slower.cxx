
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

float slowerVersion() {
  return 0.1;
}

int slowerSetup( SlowerConnection& slower, uint16_t port) {
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

int slowerRemote(  SlowerRemote& remote, char* server, uint16_t port ){
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

    std::clog << "Got remote IP of " << inet_ntoa( remote.addr.sin_addr) << std::endl;
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

int slowerSend( SlowerConnection& slower, char buf[], int bufLen, SlowerRemote& remote ) {
  assert( bufLen <= 1200 );
  assert( slower.fd > 0 );
  
  int n = sendto( slower.fd, buf, bufLen, 0 /*flags*/,
                  (struct sockaddr*)&remote.addr, remote.addrLen);
  if (n < 0) {
    perror("UDP sendto error");
    return -1;
  } else if (n != bufLen) {
    perror("UDP sendto failed");
    return -1;
  }
  
  return 0;
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

int slowerRecv( SlowerConnection& slower, char buf[], int bufSize, int* bufLen, SlowerRemote* remote ) {
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

   return 0;
}



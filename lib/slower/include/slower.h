#ifndef SLOWER_H
#define SLOWER_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

float slowerVersion(); 

const uint16_t slowerDefaultPort = 2022;

typedef struct {
    int fd;  // UDP socket
} SlowerConnection;

typedef struct {
  socklen_t addrLen;
  struct sockaddr_in addr;
} SlowerRemote;

  
int slowerSetup( SlowerConnection& slower, uint16_t port=0);

int slowerRemote(  SlowerRemote& remote, char server[], uint16_t port=slowerDefaultPort );

int slowerSend( SlowerConnection& slower, char buf[], int bufLen, SlowerRemote& remote );

int slowerWait( SlowerConnection& slower );

int slowerRecv( SlowerConnection& slower, char buf[], int bufSize, int* bufLen, SlowerRemote* remote );


#endif  // SLOWER_H

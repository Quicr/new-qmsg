#ifndef SLOWER_H
#define SLOWER_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

float slowerVersion(); 

const uint16_t slowerDefaultPort = 2022;
const uint16_t slowerMTU = 1200;


typedef struct {
  uint64_t part[2];
} ShortName;
  
typedef struct {
  socklen_t addrLen;
  struct sockaddr_in addr;
} SlowerRemote;

typedef struct {
  int fd;  // UDP socket
  SlowerRemote relay;
} SlowerConnection;


int slowerSetup( SlowerConnection& slower, uint16_t port=0);
int slowerRemote( SlowerRemote& remote, char server[], uint16_t port=slowerDefaultPort );
int slowerAddRelay( SlowerConnection& slower, SlowerRemote& remote );
int slowerClose( SlowerConnection& slower );

int slowerSend( SlowerConnection& slower, char buf[], int bufLen, SlowerRemote& remote );
int slowerWait( SlowerConnection& slower );
int slowerRecv( SlowerConnection& slower, char buf[], int bufSize, int* bufLen, SlowerRemote* remote );

int slowerPub( SlowerConnection& slower, ShortName& name, char buf[], int bufLen );
int slowerRecvPub( SlowerConnection& slower, ShortName* name, char buf[], int bufSize, int* bufLen );
int slowerSub( SlowerConnection& slower, ShortName& name, int mask );
int slowerUnSub( SlowerConnection& slower, ShortName& name, int mask );
typedef enum {
  SlowerMsgInvalid =0,
  SlowerMsgPub=1,
  SlowerMsgSub=2,
  SlowerMsgUnSub=3
} SlowerMsgType;

int slowerRecvMulti( SlowerConnection& slower, ShortName* name, SlowerMsgType type, int mask, char buf[], int bufSize, int* bufLen );


#endif  // SLOWER_H

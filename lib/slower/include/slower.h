#ifndef SLOWER_H
#define SLOWER_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

float slowerVersion(); 

const uint16_t slowerDefaultPort = 5004;
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


bool operator==( const ShortName& a, const ShortName& b );
bool operator!=( const ShortName& a, const ShortName& b );
bool operator<( const ShortName& a, const ShortName& b );

bool operator!=( const SlowerRemote& a, const SlowerRemote& b );
bool operator<( const SlowerRemote& a, const SlowerRemote& b );




typedef enum {
  SlowerMsgInvalid =0,
  SlowerMsgPub=1,
  SlowerMsgSub=2,
  SlowerMsgUnSub=3,
  SlowerMsgAck=4
} SlowerMsgType;


int slowerSetup( SlowerConnection& slower, uint16_t port=0);
int slowerRemote( SlowerRemote& remote, char server[], uint16_t port=slowerDefaultPort );
int slowerAddRelay( SlowerConnection& slower, SlowerRemote& remote );
int slowerClose( SlowerConnection& slower );

int slowerWait( SlowerConnection& slower );

int slowerPub( SlowerConnection& slower, ShortName& name, char buf[], int bufLen, SlowerRemote* remote=NULL );
int slowerAck( SlowerConnection& slower, ShortName& name, SlowerRemote* remote=NULL );
int slowerSub( SlowerConnection& slower, ShortName& name, int mask, SlowerRemote* remote=NULL );
int slowerUnSub( SlowerConnection& slower, ShortName& name, int mask, SlowerRemote* remote=NULL );

int slowerRecvPub( SlowerConnection& slower, ShortName* name, char buf[], int bufSize, int* bufLen );
int slowerRecvAck( SlowerConnection& slower, ShortName* name  );
int slowerRecvMulti( SlowerConnection& slower, ShortName* name, SlowerMsgType* type,  SlowerRemote* remote, int* mask, char buf[], int bufSize, int* bufLen );


#endif  // SLOWER_H

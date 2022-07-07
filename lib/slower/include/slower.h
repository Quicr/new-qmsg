#ifndef SLOWER_H
#define SLOWER_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

// TODO - flip this to be a class


float slowerVersion(); 

const uint16_t slowerDefaultPort = 5004;
const uint16_t slowerMTU = 1200;


//typedef struct {
//  uint64_t part[2];
//} MsgShortName;
  
typedef struct {
  socklen_t addrLen;
  struct sockaddr_in addr;
} SlowerRemote;

typedef struct {
  int fd;  // UDP socket
  SlowerRemote relay;
} SlowerConnection;

/**
 * Defines the slow-relay "MsgShortName" binary header. This is the binary
 *     encoded name from string qmsg://msg/org-<org>/team-<team>/ch-<channel>/dev-<device>/<msgNum>
 *
 *     See https://github.com/Quicr/qmsg/blob/main/doc/protocol.md for more details.
 */
#define MSG_SHORT_NAME_LEN    16
 typedef union {
     struct { // 128 bits / 16 bytes

         uint32_t         origin_id: 24;          ///< Origin ID
         u_char           app_id;                 ///< App ID
         u_char           path;                   ///< Path
         uint32_t         org: 18;                ///< Org
         uint32_t         team: 20;               ///< Team

         /* TODO:
          * Below is not common to all message short names, such as key-package, welcome, ...
          * Need to revisit the base shortname to see if the relay can use that instead of
          * having to define/understand each shortname.
          */
         uint16_t         channel: 10;            ///< Channel
         uint32_t         device: 20;             ///< Device
         uint32_t         msg_id: 20;             ///< Message ID

     } __attribute__ ((__packed__, __aligned__(1))) spec;

     u_char       data[MSG_SHORT_NAME_LEN];       ///< Buffer for the spec struct, this must be the size of the packed spec
 } __attribute__ ((__packed__, __aligned__(1))) MsgShortName;
 

// TODO: Add Key Package and Welcome ShortNames

/**
 * Defines the slow-relay message header. Every message header must start with this. Data follows
 *     the header.
 */
struct MsgHeader {
    int8_t                        type;                 ///< Message type as defined by enum SlowerMsgType
    MsgShortName                  name;
} __attribute__ ((__packed__, __aligned__(1)));

/**
 * Defines the slow-relay publish message header. This follows the slow-relay message header.
 */
struct MsgPubHeader {
    uint16_t                        dataLen;              ///< Length of the data to follow
} __attribute__ ((__packed__, __aligned__(1)));

/**
 * Defines the slow-relay subscribe message header. This follows the slow-relay message header.
 */
struct MsgSubHeader {
    int8_t                          mask;              ///< Subscriber mask
} __attribute__ ((__packed__, __aligned__(1)));


bool operator==(const MsgShortName& a, const MsgShortName& b );
bool operator!=(const MsgShortName& a, const MsgShortName& b );
bool operator<(const MsgShortName& a, const MsgShortName& b );

bool operator!=( const SlowerRemote& a, const SlowerRemote& b );
bool operator<( const SlowerRemote& a, const SlowerRemote& b );




typedef enum {
  SlowerMsgInvalid =0,
  SlowerMsgPub=1,
  SlowerMsgSub=2,
  SlowerMsgUnSub=3,
  SlowerMsgAck=4
} SlowerMsgType;


int slowerSetup( SlowerConnection& slower, const uint16_t port=0);
int slowerRemote( SlowerRemote& remote, const char server[], const uint16_t port=slowerDefaultPort );
int slowerAddRelay( SlowerConnection& slower, const SlowerRemote& remote );
int slowerClose( SlowerConnection& slower );

int slowerWait( SlowerConnection& slower );

int slowerGetFD( SlowerConnection& slower);

int slowerPub(SlowerConnection& slower, const MsgShortName& name, char buf[], int bufLen, SlowerRemote* remote=NULL );
int slowerAck(SlowerConnection& slower, const MsgShortName& name, SlowerRemote* remote=NULL );
int slowerSub(SlowerConnection& slower, const MsgShortName& name, int mask, SlowerRemote* remote=NULL );
int slowerUnSub(SlowerConnection& slower, const MsgShortName& name, int mask, SlowerRemote* remote=NULL );

int slowerRecvPub(SlowerConnection& slower, MsgShortName* name, char buf[], int bufSize, int* bufLen );
int slowerRecvAck(SlowerConnection& slower, MsgShortName* name  );
int slowerRecvMulti(SlowerConnection& slower, MsgShortName* name, SlowerMsgType* type, SlowerRemote* remote, int* mask, char buf[], int bufSize, int* bufLen );
void getMaskedMsgShortName(const MsgShortName &src, MsgShortName &dst, const int mask);

#endif  // SLOWER_H



// TODO - some sort of abstrqct buffer / vector structure to sue to relace all the arrays

// TODO - keep this all C binding so can link to Rust etc

void encodeAsciiMsg( int channel, int msgNum ,
  char msgBuff[], int msgBufLen,
    char outputBuffer[], int outputBufferSize, int* outpuleLen );


typedef enum {
  InvalidMsg=0,
  AsciiMsg
} MessageType;

MessageType decodeMessageType ( char inputBuffer[], int inputBufferLen );

int decodeAsciiMsg(
  char inputBuffer[], int inputBufferLen,
  int* channel, int* msgNum,
  char msgBuf[], int msgBufSize, int* msgBufLen
);


typedef struct {
  char bytes[8];
} ShortName;

typedef struct {
  char string[128];
  int stringLen;
} LongName;

typedef enum {
  InvalidMsgName=0,
  AsciiMsgName
} MessageNameType;

ShortName encodeName( const LongName& name );
int decodeName( const ShortName& name, LongName* result );

ShortName encodeAsciiMsgName( char domain[], int domainLen,
  int org, int team, int channel, int device, int msg
);

MessageNameType decodeNameType( const ShortName& name );

int decodeAsciiMsgName( const ShortName& name,
    char domain[], int domainSize, int* domainLen,
    int* org, int* teamm, int* channel, int* device, int* msg );

    

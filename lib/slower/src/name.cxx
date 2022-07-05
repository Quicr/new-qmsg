
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>


#include <name.h>
#include <slower.h>

Name::Name(ShortName &n) : name(n){};

Name::Name(NamePath path, uint32_t org, uint32_t team,
           uint64_t fingerprint) { // key package & welcome
}

//Name::Name(NamePath path, uint32_t org, uint32_t team, uint32_t epoch,
//           uint32_t rand) { // commit-all
//}

//Name::Name(NamePath path, uint32_t org, uint32_t team, uint32_t device,
//           uint32_t epoch, uint32_t rand) { // commit-one
//}

Name::Name(NamePath path, uint32_t org, uint32_t team, uint16_t channel,
           uint32_t device, uint32_t msgNum) { // name for message data
  assert(path == NamePath::message);
  assert(org <= 0x3ffff);    // 18 bits
  assert(team <= 0xFffff);   // 20 bits
  assert(channel <= 0x3FF);  // 10 bits
  assert(device <= 0xFffff); // 20 bits
  assert(msgNum <= 0xFffff); // 20 bits

  uint64_t i = itad; i <<= ( 64 -24 ); // 40
  uint64_t a = qmsgAppID; a <<= ( 64-24-8 ); // 32
  uint64_t p = (uint8_t)path; p <<= ( 64-24-8-8 ); // 24
  uint64_t o = org; o <<= ( 64-24-8-8-18 ); // 6
  uint64_t th = team >> (20-6); // top 6 bits of 20 
  uint64_t tl = (team&0x7F); tl <<= (64-14); // bot 14 bits at 50
  uint64_t c = channel; c <<= ( 64-14-10 ); // 40
  uint64_t d = device; d <<= ( 64-14-10-20 ); // 20
  uint64_t n = msgNum;

  name.part[1] = i | a | p | o | th;
  name.part[0] = tl | c | d | n;
}

uint32_t Name::orginID() {
  uint64_t r = name.part[1];
  r >>= 64-24;
  r &= 0xFFffff; // 24 bits 
  return (uint32_t)(r);
};
uint8_t Name::appID() {
  uint64_t r = name.part[1];
   r >>= 64-24-8;
  r &= 0xFF; // 8 bits 
  return (uint8_t)(r);
};
NamePath Name::path() {
  uint64_t r = name.part[1];
  r >>= 64-24-8-8;
  r &= 0xFF; // 8 bits 
  return (NamePath)(r);
};
uint32_t Name::org() {
  uint64_t r = name.part[1];
  r >>=  64-24-8-8-18;
  r &=  0x3ffff; // 18 bits 
  return (uint32_t)(r);
};
uint32_t Name::team() {
  uint64_t rh = name.part[1];
  rh &=  0x3F; // 6 bits
  uint64_t rl = name.part[0];
  rl >>= 64-14;
  rl &= 0x3F; // 14 bits
  uint64_t r = (rh<<14) | rl;
  r &= 0xFffff; // 20 bits 
  return (uint32_t)(r);
};

uint32_t Name::channel() {
  uint64_t r = name.part[0];
  r >>=  64-14-10;
  r &=  0x3FF; // 10 bits 
  return (uint32_t)(r);
};

uint32_t Name::device() {
  uint64_t r = name.part[0];
  r >>=  64-14-10-20;
  r &=  0x3ffff; // 20 bits 
  return (uint32_t)(r);
};

uint32_t Name::msgNum() {
  uint64_t r = name.part[0];
  r &=  0x3ffff; // 20 bits 
  return (uint32_t)(r);
};

uint64_t Name::fingerprint() {
  uint64_t r = name.part[0];
  assert(0); // TODO 
  return (uint64_t)(r);
};

uint32_t Name::epoch() {
  uint64_t r = name.part[0];
  assert(0); // TODO 
  return (uint32_t)(r);
};

uint32_t Name::rand() {
  uint64_t r = name.part[0];
  assert(0); // TODO 
  return (uint32_t)(r);
};

ShortName &Name::shortName() { return name; }

std::string Name::shortString() {
  std::stringstream ss;
  ss << std::hex << std::setfill('0') 
     << std::setw( 8 ) << ((name.part[1] >> 32) & 0xFFFFffff) << "-"
     << std::setw( 8 ) << ((name.part[1] >> 0) & 0xFFFFffff) << "-"
     << std::setw( 8 ) << ((name.part[0] >> 32) & 0xFFFFffff) << "-"
     << std::setw( 8 ) << ((name.part[0] >> 0) & 0xFFFFffff)
     << std::dec;

  return ss.str();
}

std::string Name::longString() {
  std::stringstream ss;
  if ( ( orginID() != itad ) || ( appID() != qmsgAppID ) ) {
    ss << "unkown-" << shortString() ; 
  } else {
    switch ( path() ) {
    case NamePath::message: {
      ss << "qmsg://mesg"
         << "/org-" << org()
         << "/team-" << team()
         << "/ch-" << channel()
         << "/dev-" << device() 
         << "/msg-" << msgNum();
    }
      break;
    default:
      break;
    }
  }
  
  return ss.str();
};

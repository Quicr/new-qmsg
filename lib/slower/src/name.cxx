
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>


#include <name.h>
#include <slower.h>

Name::Name(MsgShortName &n) : name(n){};

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

  name.spec.origin_id = itad;
  name.spec.app_id = qmsgAppID;
  name.spec.path = (uint8_t)path;
  name.spec.org = org;
  name.spec.team = team;
  name.spec.channel = channel;
  name.spec.device = device;
  name.spec.msg_id = msgNum;
}

uint32_t Name::orginID() {
  return (uint32_t)(name.spec.origin_id);
};
uint8_t Name::appID() {
  return (uint8_t)(name.spec.app_id);
};
NamePath Name::path() {
  return (NamePath)(name.spec.path);
};
uint32_t Name::org() {
  return (uint32_t)(name.spec.org);
};
uint32_t Name::team() {
  return (uint32_t)(name.spec.team);
};
uint32_t Name::channel() {
  return (uint32_t)(name.spec.channel);
};

uint32_t Name::device() {
  return (uint32_t)(name.spec.device);
};

uint32_t Name::msgNum() {
  return (uint32_t)(name.spec.msg_id);
};

MsgShortName &Name::shortName() { return name; }

std::string Name::shortString() {
  std::stringstream ss;

  uint32_t *data = (uint32_t *)&name.data;

  ss << std::hex << std::setfill('0') 
     << std::setw( 8 ) << data[0] << "-"
     << std::setw( 8 ) << data[1] << "-"
     << std::setw( 8 ) << data[2] << "-"
     << std::setw( 8 ) << data[3]
     << std::dec;

  return ss.str();
}

std::string Name::longString() {
  std::stringstream ss;
  if ( ( orginID() != itad ) || ( appID() != qmsgAppID ) ) {
    ss << "unknown-" << shortString() ;
  } else {
    switch ( path() ) {
    case NamePath::message: {
      ss << "origin_id: " << orginID()
         << " qmsg://msg"
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

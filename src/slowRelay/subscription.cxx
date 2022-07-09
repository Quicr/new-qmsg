

#include <cassert>
#include <iostream>

#include <slower.h>

#include "subscription.h"


Subscriptions::Subscriptions() {
  subscriptions.resize(128);
}

void Subscriptions::add(const MsgShortName& name, const int mask, const SlowerRemote& remote ) {
  assert( mask <= 70 ); // Mask to org

  MsgShortName group;
  getMaskedMsgShortName(name, group, mask);

  //std::clog << "subscribe.cxx: add "
  //          << std::hex << group.part[1] << "-" <<  group.part[0] << std::dec
  //          << " port=" <<  ntohs( remote.addr.sin_port )
  //          << std::endl;
  
  auto mapPtr =  subscriptions[mask].find( group );
  if ( mapPtr == subscriptions[mask].end() ) {
    std::set<SlowerRemote> list;
    list.insert( remote );
    std::pair<MsgShortName,std::set<SlowerRemote>> pair;
    pair = make_pair( group , list );
    subscriptions[mask].insert( pair );
  }
  else {
    std::set<SlowerRemote>& list = mapPtr->second;
    if ( list.find( remote ) == list.end() ) {
      list.insert( remote ); // TODO - rethink if list is right thing here 
    }
  }
}
  
void Subscriptions::remove(const MsgShortName& name, const int mask, const SlowerRemote& remote ) {
  assert( mask <= 64 );
  MsgShortName group;
  getMaskedMsgShortName(name, group, mask);

  auto mapPtr = subscriptions[mask].find( group );
  if ( mapPtr != subscriptions[mask].end() ) {
    std::set<SlowerRemote>& list = mapPtr->second;
    if ( list.find( remote ) == list.end() ) {
      list.erase( remote ); 
    }
  }
}
  
std::list<SlowerRemote> Subscriptions::find(  const MsgShortName& name  ) {
  std::list<SlowerRemote> ret;
  //std::clog << "subscribe.cxx: find " << std::hex << group.part[1] << "-" <<  group.part[0] << std::dec << std::endl;
  MsgShortName group;

  // TODO: Fix this to not have to iterate for each mask bit
  for ( int mask=0; mask <=70 ; mask ++ ) {
    getMaskedMsgShortName(name, group, mask);

    auto mapPtr = subscriptions[mask].find( group );
    if ( mapPtr != subscriptions[mask].end() ) {
      std::set<SlowerRemote>& list = mapPtr->second;
      for( const SlowerRemote& remote : list ) {
        SlowerRemote dest = remote;
        ret.push_back( dest );
        //std::clog << "subscribe.cxx:     found port=" <<  ntohs( remote.addr.sin_port )<< std::endl;
      }
    }
  }
  return ret;
}




#include <cassert>
#include <iostream>
#include <cstring>

#include "cache.h"


void Cache::put(const MsgShortName& name, const std::vector<uint8_t>& data ) {
  assert( data.size() > 0 );

  std::vector<uint8_t>* vec = new std::vector<uint8_t>( data );

  std::pair<MsgShortName, std::vector<uint8_t>* > pair;
  pair = make_pair( name, vec );

  dataCache.insert( pair );
}


const std::vector<uint8_t>* Cache::get( const MsgShortName& name ) const {
  auto mapPtr = dataCache.find( name );
  if ( mapPtr == dataCache.end() ) {
    return &emptyVec;
  }

  const std::vector<uint8_t>* vecP =  mapPtr->second;
  assert( vecP );
  assert( vecP->size() > 0 );

  return  vecP;
}


bool Cache::exists(  const MsgShortName& name ) const {
  auto mapPtr = dataCache.find( name );
  if ( mapPtr == dataCache.end() ) {
    return false;
  }
  return true; 
}


std::list<MsgShortName> Cache::find(const MsgShortName& name, const int mask ) const {
  std::list<MsgShortName> ret;
  assert( mask <= 70 ); // TODO

  MsgShortName startName = name;
  getMaskedMsgShortName(name, startName, mask);

  //std::clog << "  Cache::find lower = " << std::hex << startName.part[1] << "-" <<  startName.part[0] << std::dec << std::endl;
  //std::clog << "  Cache::find upper = " << std::hex << endName.part[1] << "-" <<  endName.part[0] << std::dec << std::endl;
  MsgShortName endName =  startName;
  std::memset(endName.data + MSG_SHORT_NAME_LEN - (mask / 8), 0xff, (mask / 8));

  auto start = dataCache.lower_bound( startName );

  auto end = dataCache.upper_bound( endName );
  
  for ( auto it = start; it != end; it++ ) {
    MsgShortName dataName = it->first;

    //std::cerr << "   Cache::find adding " << std::hex << dataName.part[1] << "-" <<  dataName.part[0] << std::dec << " to results" << std::endl;

    assert( it->second );
    assert( it->second->size() > 0 );
    
    ret.push_back( dataName );
  }
  
  return ret;
}


Cache::~Cache(){
  for( auto it =  dataCache.begin(); it != dataCache.end(); it++ ) {
    assert( it->second );
    delete it->second;
    dataCache.erase( it );
  }
  dataCache.clear();
}

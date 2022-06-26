
#include <list>
#include <set>
#include <map>
#include <vector>

#include <slower.h>


class Cache {
public:
  void put( const ShortName& name,  const std::vector<uint8_t>& data );

  const std::vector<uint8_t>* get( const ShortName& name ) const;

  bool exists(  const ShortName& name ) const;

  std::list<ShortName> find(  const ShortName& name, const int mask ) const;

  ~Cache();
  
private:
  std::map< ShortName, std::vector<uint8_t>* > dataCache;
  const std::vector<uint8_t> emptyVec;
};

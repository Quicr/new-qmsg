

#include <list>
#include <set>
#include <map>
#include <vector>

#include <slower.h>


class Subscriptions {
public:

  Subscriptions();
  
  void add(  const ShortName& name, const int mask, const SlowerRemote& remote );
  
  void remove(  const ShortName& name, const int mask, const SlowerRemote& remote );  
  
  std::list<SlowerRemote> find(  const ShortName& name  ) ;
    
 private:
  std::vector< std::map<ShortName,std::set<SlowerRemote>> > subscriptions;
};



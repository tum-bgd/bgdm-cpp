#ifndef LRU_H_INC
#define LRU_H_INC

#include<unordered_map>
#include<list>
#include<optional>

template<typename K, typename V, size_t Capacity>
class LRUCache {
public:
 static_assert(Capacity > 0);
  K last_key;
 
  
 bool put(const K& k, const V& v){
   if(index.count(k))
     return false; // it is in the list
   if (items.size() == Capacity){
     std::cout << "GARBAGE COLLECTED: " << items.back().first << std::endl;
     delete items.back().second; // @hack: unstable as smart pointers did not fit splicing
     index.erase(items.back().first); 
     items.pop_back();
     
   }
   last_key = k;
 items.emplace_front(k, v);
 index.emplace(k, items.begin());
 return true;
 }


 std::optional<V> get(const K& k)
   {
     //if (last_key == k) return items.begin()->second;
     auto itr = index.find(k);
     if(itr == index.end()) {
       return {}; //empty std::optional
     }
     

 /*Use list splice to transfer this item to
  the first position, which makes the item
  most-recently-used. Iterators still stay valid.*/
 items.splice(items.begin(), items, itr->second);

 //Return the value in a std::optional
 return itr->second->second;

   }

 //Erases an item
 void erase(const K& k)
 {
   auto itr = index.find(k);
 if(itr == index.end()) {
  return;
 }
 items.erase(itr->second);
 index.erase(itr);
 }

 //Utility function.
 //Calls callback for each {key,value}
 template<typename C>
 void forEach(const C& callback) const {
   for(auto& [k,v] : items) {
    callback(k, v);
   }
 }
  size_t size(){
    return index.size(); 
  }

public:
 /*std::list stores items (pair<K,V>) in
 most-recently-used to least-recently-used order.*/
 std::list<std::pair<K,V>> items;

 //unordered_map acts as an index to the items store above.
 std::unordered_map<K, typename std::list<std::pair<K,V>>::iterator> index;
};
#endif

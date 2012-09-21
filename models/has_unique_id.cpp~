#include "has_unique_id.hpp"

namespace Scan {
    int HasUniqueId::counter = 0;
    HasUniqueId::HasUniqueId() : id(++counter) {}
    HasUniqueId::HasUniqueId(int i) {
        set_id(i);
    }

    HasUniqueId::HasUniqueId(const HasUniqueId& other) :
        id(other.id) {}
    HasUniqueId& HasUniqueId::operator=(const HasUniqueId& other) 
    {
        id = other.id;
    }

   void HasUniqueId::set_id(int i) 
   {
        id = i;
        counter = (counter > i) ? counter : (i+1);
    }

    
    bool LessUniqueId::operator()(const HasUniqueId& a, const HasUniqueId& b) const 
    {
        return a.get_id() < b.get_id();
    }

}


#include "has_unique_id.hpp"

namespace Scan {
    int HasUniqueId::counter = 0;
    HasUniqueId::HasUniqueId() : id(++counter) {}
    HasUniqueId::HasUniqueId(const HasUniqueId& other) :
        id(other.id) {}
    HasUniqueId& HasUniqueId::operator=(const HasUniqueId& other) 
    {
        id = other.id;
    }

    bool LessUniqueId::operator()(const HasUniqueId& a, const HasUniqueId& b) const 
    {
        return a.get_id() < b.get_id();
    }

}


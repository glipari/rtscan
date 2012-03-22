#include <algorithm>

#include <common/property.hpp>

using namespace std;

namespace Scan {
    std::map<std::string, std::unique_ptr<BaseProperty> > PropertyFactory::reg;

    std::unique_ptr<BaseProperty> PropertyFactory::create(const std::string &type)
    {
        auto i = reg.find(type);
        if ( i != reg.end() ) 
            return unique_ptr<BaseProperty>(i->second->clone());
        else throw PropertyNotFound(type);
    }
 
    void PropertyFactory::reg_in_fact(const std::string &type, BaseProperty* prop)
    {
        reg.insert(make_pair(type, prop));
    }

#define PROPERTY_REG_STUB(X,V) Scan::PropertyFactoryStub<X> obj_##X(STRINGIFY(X), V)

    Scan::PropertyFactoryStub<int> obj_int("int", 0);
    PROPERTY_REG_STUB(float, 0.0);
    PROPERTY_REG_STUB(unsigned, 0);
 
    PropertySet::PropertySet() : myset(new SET()) {}

    PropertySet::~PropertySet()
    {
    	for (auto i : *myset) delete i.second;
    }
    
    void PropertySet::swap(PropertySet &other)
    {
    	myset.swap(other.myset);
    }
    
    PropertySet::PropertySet(const PropertySet &other) : myset(new SET())
    {
    	for (auto i : *(other.myset))
			myset->insert(make_pair(i.first, i.second->clone()));
    }

    PropertySet & PropertySet::operator=(const PropertySet &other)
    {
    	if (this != &other) {
    		PropertySet temp;
    		for (auto i : *(other.myset))
    			temp.myset->insert(make_pair(i.first, i.second->clone()));

    		swap(temp);
    	}
    	return *this;
    }

    bool PropertySet::contains(const std::string &s)
    {
    	auto i = myset->find(s);
        if (i != myset->end()) return true;
        else return false;
    }
}

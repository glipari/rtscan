#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <string>
#include <vector>
#include <memory>
#include <cstdlib>

#include <common/exceptions.hpp>

#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

namespace Scan {

    /// Exception: cannot find a property
    DECL_EXC(PropertyNotFound);

    /// Exception: the property has the wrong type
    DECL_EXC(PropertyWrongType);

    /**
       @todo documentation 
     */
    struct Property { 
        std::string name; 
        std::string value;

        Property() {}
        Property(const std::string &n, const std::string &v = "") :
            name(n), value(v) {}       

        std::string get_value() const { return value; }
        void set_value(const std::string &v) { value = v; }
        std::string get_name() const { return name; }
        int get_int() const { return std::atoi(get_value().c_str()); } 
        double get_double() const { return std::atof(get_value().c_str()); } 
    };
    

    /**
       @todo Maybe to be renamed into PropertyTree ? 

       Need functions to search the tree for a variable with a
       specific name or type. Example of searches (each one takes a
       root node, that is an Element):

       - give me (copy) the subtree with root at a certain node

       - find the element with a certain name/type among the direct
         childrens

       - find the element with a certain name/type in the entire
         (sub)-tree

       - find the element with a certain name/type which has a father
         with name/type x
    */    
    struct PropertyList { 
        typedef boost::variant< Property, 
                                boost::recursive_wrapper<PropertyList> 
                                > Element;
        
        std::string name;
        std::string type;
        std::vector< Element > children;
    
        PropertyList() {}
        PropertyList(const std::string &n, const std::string &t) :
            name(n), type(t) {}
        
        size_t size() { return children.size(); }
        
        Element get(unsigned i) { return children.at(i); } 
        std::string get_type()  { return type; }
        std::string get_name()  { return name; }
    };

    typedef std::vector<PropertyList> PropertySet;
}

#endif

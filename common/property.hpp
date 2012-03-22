#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <string>
#include <map>
#include <memory>

#include <common/exceptions.hpp>

namespace Scan {
    DECL_EXC(PropertyNotFound);
    DECL_EXC(PropertyWrongType);

    /** Base abstract call to represent a property. Properties are polymorphic objects, so
     * they can be copied via a clone() virtual functions */
    class BaseProperty {
    protected:
        std::string name;
    public:
        BaseProperty(const std::string &s) : name(s) {}
        virtual ~BaseProperty() {}
        /** returns a copy of the object */
        virtual BaseProperty *clone() = 0;
        std::string get_name() { return name; }
    };
    
    /** A property that contains a value of type T */
    template <class T> 
    class Property : public BaseProperty {
        T value;
    public:
        Property(const std::string &s, const T& v) : BaseProperty(s), value(v) {}
        Property(const std::pair<std::string, T> &p) : BaseProperty(p.first), value(p.second) {}
        
        void get(T &v) const { v = value; }
        T get() const { return value; }
        void set(const T &v) { value = v; }
        Property<T> *clone() { return new Property<T>(name, value); }
    };

    class PropertyFactory {
        static std::map<std::string, std::unique_ptr<BaseProperty> > reg;
    public:        
        static std::unique_ptr<BaseProperty> create(const std::string &t);
        static void reg_in_fact(const std::string &t, BaseProperty* prop);
    };

    template<class T>
    class PropertyFactoryStub {
    public:
        PropertyFactoryStub(const std::string type, const T &v) {
            PropertyFactory::reg_in_fact(type, new Property<T>("stub", v));
        }
    };
    

    /** A set of properties, it can contain properties of different type values.
     * properties are accessible by name */
    class PropertySet {
    public:
        PropertySet();
        ~PropertySet();
        
        PropertySet(const PropertySet &other);
        PropertySet& operator=(const PropertySet &other);

        bool contains(const std::string &p);

        template <class T>
        void set_property(const std::string &p, const T& v);

        template <class T>
        void get_property(const std::string &p, T& v) const;

        template <class T>
        T get_property(const std::string &p) const;

    private:
        typedef std::map<std::string, BaseProperty *> SET;
        typedef std::map<std::string, BaseProperty *>::iterator ITER;

        std::unique_ptr< SET > myset;

        void swap(PropertySet &other);
    };

    template <class T>
    void PropertySet::set_property(const std::string &name, const T& v)
    {
    	auto i = myset->find(name);
    	if (i != myset->end()) {
            Property<T> *prop = dynamic_cast< Property<T> *>(i->second);
            if (!prop) throw PropertyWrongType("Property " + name + " exists with a different type");
            else prop->set(v);
    	}
    	else myset->insert(make_pair(name, new Property<T>(name, v)));
    }

    template <class T>
    void PropertySet::get_property(const std::string &name, T& v) const
    {
    	auto i = myset->find(name);
    	if (i == myset->end()) throw PropertyNotFound(name);
    	else {
            Property<T> *prop = dynamic_cast< Property<T> *>(i->second);
            if (!prop) throw PropertyWrongType("Property " + name + " exists with a different type");
            prop->get(v);
    	}
    }

    template <class T>
    T PropertySet::get_property(const std::string &name) const
    {
    	auto i = myset->find(name);
    	if (i == myset->end()) throw PropertyNotFound(name);
    	else {
            Property<T> *prop = dynamic_cast< Property<T> *>(i->second);
            if (!prop) throw PropertyWrongType("Property " + name + " exists with a different type");
            return prop->get();
    	}
    }
}

#endif

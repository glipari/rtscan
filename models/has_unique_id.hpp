#ifndef __HAS_UNIQUE_ID_HPP__
#define __HAS_UNIQUE_ID_HPP__

namespace Scan {
    /**
       This template class implements the CRTP for providing the
       "unique id" feature to a class.  Class HasUniqueId provides a
       copy constructor, an assignment operator, and a get_id() member
       function.  

       Essentially, if you want to implement a static counter of
       created objects with unique ids in your class X, you must
       publicly derive your class X from HasUniqueId<X>, and
       initialize the counter like this:

       template<> int HasUniqueId<X>::counter = 0;

       See task.hpp/cpp for an example of usage. 

       Notice that the template parameter X is never used inside
       HasUniqueId<X>: its only purpouse is to provide a new type (and a
       different counter) for each class.
    */
    template <class X> 
    class HasUniqueId { 
        int id; 
        static int counter; 
    protected: 
        void set_id(int i) { 
            id = i; 
            counter = (counter > i) ?  counter : (i+1); 
        } 
    public: 
        HasUniqueId() : id(++counter) {} 
        HasUniqueId(int i) { set_id(i); }
			
        HasUniqueId(const HasUniqueId& other) : id(other.id) {}
        
        HasUniqueId& operator=(const HasUniqueId& other) {
            id = other.id;
            return *this;
        }

        inline int get_id() const { return id; }
    };


    /**
       Function object to compare two objects derived from
       HasUniqueId<X> with their ids.
     */
    template <class X>
    class LessUniqueId {
    public:
        bool operator()(const HasUniqueId<X>& a, const HasUniqueId<X> & b) const {
            return a.get_id() < b.get_id();
        }
    };
    

    /**
       This predicate is true when the two obejcts have the same ids.
       The only way for two objects to have the same id is that one is
       a copy (with copy constructor or assignement operator) of the
       other. 
     */
    template <class X>
    class EqualIdPred {
        int id;
    public:
        EqualIdPred(const HasUniqueId<X>& x) : id(x.get_id()) {}
        bool operator()(const HasUniqueId<X>& y) const {
            return id == y.get_id();
        }
    };

    template<class X>
    bool cmp_ids(const HasUniqueId<X> &a, const HasUniqueId<X> &b)
    {
        return a.get_id() == b.get_id();
    }

}

#endif

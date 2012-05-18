#ifndef __HAS_CRITICAL_SECTION_HPP
#define __HAS_CRITICAL_SECTION_HPP

#include <vector>
#include <list>
#include <set>

#include <models/has_unique_id.hpp>

namespace Scan {
    class Resource : public HasUniqueId {
        //int id_;
        bool isShort_;
        int ceiling_;
     public:
        explicit Resource(bool s = false);        

        Resource(int rid, bool s);        
        int get_ceiling() const {return ceiling_; }
        void set_ceiling(int ceil) { ceiling_ = ceil; }
        bool is_short()  const { return isShort_; }
        //int get_id() const { return id_; }
    };

    class ResourceIdLessThan {
    public:
        bool operator()(const Resource &a, const Resource &b) const {
            return a.get_id() < b.get_id();
        }
    };

    class ResourceIdEqualPred {
        int id;
    public:
        ResourceIdEqualPred(const Resource &res) : id(res.get_id()) {}
        ResourceIdEqualPred(int rid) : id(rid) {}
        bool operator()(const Resource &res) const {
            return res.get_id() == id;
        } 
    };

    template<class Iter>
    Resource get_res_by_id(int rid, Iter begin, Iter end)
    {
        Iter r_it = find_if(begin, end, ResourceIdEqualPred(rid));
        if (r_it == end) throw "Resource not found!";
        return *r_it;
    }

    class CriticalSection;
    typedef std::vector<CriticalSection> CSSet;
    typedef std::list<const CriticalSection *>CSList;

    class CriticalSection {
        int res_id;
        double duration;
        CSSet nested;
        CriticalSection *parent; 

        mutable std::list<const CriticalSection *> cs_list;
        mutable std::list<const CriticalSection *>::const_iterator pos;
    public:        
        CriticalSection(int rid, double dur, CriticalSection *p=0);
        
        CriticalSection(const CriticalSection &cs);
        CriticalSection & operator=(const CriticalSection &cs);

        /** makes a copy of the parameter */
        void addNestedCS(const CriticalSection &cs);

        // /** Returns the parent CS, or nullptr */
        CriticalSection *get_parent() const;
        
        /** returns resource id */
        int get_resource() const;
        
        /** returns cs lenght */
        double get_duration() const;

        /** returns the iterator to the first nested critical section */
        CSSet::const_iterator begin() const; 
        /** returns the iterator to the end of the set of nested
         * critical section (points to a unvalid element) */
        CSSet::const_iterator end() const;
        
        /** returns true if the resource is accessed in this critical
            section or in any of the nested critical sections */
        bool access_resource(int r) const; 

        /** finds the first critical section in the tree that uses the resource 
            res (it uses depth-first, in-order) */ 
        const CriticalSection * find_first(int res) const;
        
        /** finds the next critical section in the tree that uses the 
            resource res (it uses depth-first, in-order) */ 
        const CriticalSection * find_next() const;
        
        /** returns the list of pointers to the containing critical
            sections, from the root to the parent. If this is an outer
            critical section, returns an empty list */
        CSList get_path() const;
    };

    /** find the critical section in the list that accesses res (if any) */ 
    template<class Iter>
    Iter find_cs(Iter b, Iter e, int res) {
        Iter i = b;
        while (i != e) {
            if ((*i)->get_resource() == res) return i;
            i++;
        }
        return e;
    }

    class HasCriticalSection {
    private:
        static double get_dur(const std::vector<CriticalSection> &v, int rid); 
        CSSet cs;
    public:

        HasCriticalSection() : cs() {}
        void addCS(const CriticalSection &c);
        CSSet get_outer_cs() const;

        /** returns true if the resource is accessed by the task in
         * any of the critical sections (also nested ones) */
        bool uses_resource(int res_id) const;
        
        /** get the list of pointers to all critical sections of this
            task that insist on resource res */
        CSList get_cs_list(int res) const;

        /** get longest critical section on resource res_id */
        double get_max_length(int res_id) const;
    };

    template<class Iter>
    Iter find_task_uses_res(Iter a, Iter b, int res)
    {
        for(Iter i = a; i!=b; ++i) {
            if (i->uses_resource(res)) 
                return i;
        }
        return b;
    }

    template<class Container, class Iter>
    void subset_tasks_use_res(Iter a, Iter b, Container &c, int res)
    {
        Iter i = a;
        i = find_task_uses_res(a, b, res);
        while (i != b) {
            c.insert(c.end(), *i);
            i = find_task_uses_res(i+1, b, res);
        }
    }
}
#endif // __HAS_CRITICAL_SECTION_HPP

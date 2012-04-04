#ifndef __HAS_CRITICAL_SECTION_HPP
#define __HAS_CRITICAL_SECTION_HPP

#include <vector>
#include <set>
#include <list>

namespace Scan {
    class Resource {
        int id_;
        bool isShort_;
        int ceiling_;
     public:
        Resource(int rid, bool s = false);        
        int get_ceiling() const {return ceiling_; }
        void set_ceiling(int ceil) { ceiling_ = ceil; }
        bool isShort()  const { return isShort_; }
        int get_id() const { return id_; }
    };

    class CriticalSection;
//    typedef std::vector<CriticalSection *> CSSet;
    typedef std::vector<CriticalSection> CSSet;

    class CriticalSection {
        int res_id;
        double duration;
        CSSet nested;
        CriticalSection *parent; 

        std::list<const CriticalSection *> cs_list;
        std::list<const CriticalSection *>::const_iterator pos;
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
        const CriticalSection * find_first(int res);
        
        /** finds the next critical section in the tree that uses the 
            resource res (it uses depth-first, in-order) */ 
        const CriticalSection * find_next();        
    };

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
        bool uses_resource(int res_id);
    };

    class ChainElem {
    public: 
        HasCriticalSection *task1;
        CriticalSection *cs;
        HasCriticalSection *task2;
        ChainElem(HasCriticalSection *t1, CriticalSection *c, HasCriticalSection *t2);
    };
    
    typedef std::vector<ChainElem> BlockingChain;
    typedef std::set<HasCriticalSection *> HCSSet;
    typedef std::set<Resource *> ResourceSet;
    
    std::set<BlockingChain> getAllBlockingChains(const HasCriticalSection &t);    
    HCSSet getBlockingTasks(const BlockingChain &bc);
}
#endif // __HAS_CRITICAL_SECTION_HPP

#ifndef __HAS_CRITICAL_SECTION_HPP
#define __HAS_CRITICAL_SECTION_HPP

#include <vector>
#include <set>

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
    typedef std::set<CriticalSection *> CSSet;

    class CriticalSection {
    public:
        int res_id;
        double duration;
        std::vector<CriticalSection> nested;
        CriticalSection *parent; 
    
        CriticalSection(int rid, double dur, CriticalSection *p=0) : 
            res_id(rid), duration(dur), parent(p) {}
        
        /** just stores the pointers, used for analysis */
        void addCStoSet(CSSet &s);

        /** makes a copy of the parameter */
        void addNestedCS(const CriticalSection &cs);
    };

    
    class HasCriticalSection {
    private:
        static double get_dur(const std::vector<CriticalSection> &v, int rid); 
    public:
        std::vector<CriticalSection> cs;

        HasCriticalSection() : cs() {}
        /** 
            Returns the worst case duration of the longest critical
            section on resource rid, or 0 if the task does not use
            resource rid
        */
        double getDuration(int rid);
        CSSet getAllCriticalSections();
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

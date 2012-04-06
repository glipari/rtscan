#include "has_critical_section.hpp"

using namespace std;

namespace Scan {
    Resource::Resource(int rid, bool s) :
        id_(rid), isShort_(s), ceiling_(0) {}
    
    CriticalSection::CriticalSection(int rid, double dur, CriticalSection *p) : 
        res_id(rid), duration(dur), nested(), parent(p) 
    {}
    
    CriticalSection::CriticalSection(const CriticalSection &cs) : 
        res_id(cs.res_id), 
        duration(cs.duration),
        nested(cs.nested),
        parent(nullptr),
        cs_list(),
        pos(cs_list.end())
    {
        // set the parent of all nested critical sections
        for (auto i = nested.begin(); i != nested.end(); i++)  
            i->parent = this;
    }

    CriticalSection & CriticalSection::operator=(const CriticalSection &cs)
    {
        res_id = cs.res_id;
        duration = cs.duration;
        nested = cs.nested;

        for (auto i = nested.begin(); i != nested.end(); i++)  
            i->parent = this;

        parent = nullptr;

        cs_list.clear();
        pos = cs_list.end();

        return *this;
    }

    void CriticalSection::addNestedCS(const CriticalSection &c)
    {
        nested.push_back(c);
        // after adding the critical section, I have to set its parent. 
        // it may happen that the resize operation on the vector actually changed 
        // the pointers. hence I have to set again the parent for 
        // everybody
        for (auto i = nested.begin(); i != nested.end(); i++)  
            i->parent = this;
    }

    CriticalSection *CriticalSection::get_parent() const
    {
        return parent;
    }
        
    int CriticalSection::get_resource() const
    {
        return res_id;
    }
        
    double CriticalSection::get_duration() const
    {
        return duration;
    }
        
    CSSet::const_iterator CriticalSection::begin() const
    {
        return nested.begin();
    } 
            
    CSSet::const_iterator CriticalSection::end() const
    {
        return nested.end();
    }
            
    bool CriticalSection::access_resource(int r) const 
    {
        if (res_id == r) return true;
        else {
            for (auto i = nested.begin(); i!= nested.end(); ++i) 
                if (i->access_resource(r)) return true;
        }
        return false;
    }
            
    void __visit_pre_order(const CriticalSection *cs, int res, list<const CriticalSection *> &l)
    {
        if (res == cs->get_resource()) 
            l.push_back(cs);
        for (auto i = cs->begin(); i!= cs->end(); i++) 
            __visit_pre_order(&(*i), res, l);
    }
 
    /** finds the first critical section in the tree that uses the resource 
        res (it uses depth-first, pre-order) */ 
    const CriticalSection * CriticalSection::find_first(int res) const
    {
        // builds the list
        cs_list.clear();
        __visit_pre_order(this, res, cs_list);
        if (cs_list.size() == 0) {
            pos = cs_list.end();
            return nullptr;
        }
        else {
            pos = cs_list.begin();
            return *(pos++);
        }
    }
        
    /** finds the next critical section in the tree (after previos) that uses the 
        resource res (it uses depth-first, pre-order) */ 
    const CriticalSection * CriticalSection::find_next() const
    {
        if (pos == cs_list.end()) return nullptr;
        else return *(pos++);
    }

    CSList CriticalSection::get_path() const 
    {
        CSList l;
        const CriticalSection *p = parent;
        while (p != nullptr) {
            l.push_front(p);
            p = p->parent;
        }
        return l;
    }
  
    CSSet HasCriticalSection::get_outer_cs() const
    {
        CSSet s;
        for (auto i = cs.begin(); i != cs.end(); i++)  
            s.push_back(*i);
        return s;        
    }

    void HasCriticalSection::addCS(const CriticalSection &c)
    {
        cs.push_back(c);
    }

    bool HasCriticalSection::uses_resource(int res_id) const
    {
        for (auto i = cs.begin(); i != cs.end(); ++i) {
            if (i->access_resource(res_id)) return true;
        }
        return false;
    }

    CSList HasCriticalSection::get_cs_list(int res) const
    {
        const CriticalSection *ptr;
        CSList l;
        for (auto i = cs.begin(); i != cs.end(); ++i) {
            ptr = i->find_first(res);
            while (ptr != nullptr) {
                l.push_back(ptr);
                ptr = i->find_next();
            }
        }
        return l;
    }

    // void BlockingChain::addTask(const HasUniqueId &t)
    // {
    //     tasks.insert(t.get_id());
    // }

    // void BlockingChain::addRes(int r)
    // {
    //     res.insert(r);
    // }
        
    // BlockingChain::task_iterator BlockingChain::task_begin() const
    // {
    //     return tasks.begin();
    // }

    // BlockingChain::task_iterator BlockingChain::task_end() const
    // {
    //     return tasks.end();
    // }

    // BlockingChain::res_iterator BlockingChain::res_begin() const
    // {
    //     return res.begin();
    // }
    
    // BlockingChain::res_iterator BlockingChain::res_end() const
    // {
    //     return res.end();
    // }
}

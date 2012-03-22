#include "has_critical_section.hpp"

using namespace std;

namespace Scan {

    Resource::Resource(int rid, bool s) :
        id_(rid), isShort_(s), ceiling_(0) {}


    void CriticalSection::addCS(CSSet &s)
    {
        for (auto c : nested) {
            s.insert(&c);
            c.addCS(s);
        }
    }

    double HasCriticalSection::get_dur(const std::vector<CriticalSection> &v, int rid)
    {
        vector<double> d;
        for (auto c : v) {
            if (c.res_id == rid) d.push_back(c.duration);
            else d.push_back( get_dur(c.nested, rid) );
        }
        if (d.size() == 0) return 0;
        else return *max(d.begin(), d.end());
    }


    double HasCriticalSection::getDuration(int rid)
    {
        return get_dur(cs, rid);
    } 

    CSSet HasCriticalSection::getAllCriticalSections()
    {
        CSSet s;
        for (auto c : cs) {
            s.insert(&c);
            c.addCS(s)
        }
    }

    ChainElem::ChainElem(HasCriticalSection *t1, CriticalSection *c, HasCriticalSection *t2) :
        task1(t1), cs(c), task2(t2) {}



}

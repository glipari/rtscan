#ifndef __HAS_CRITICAL_SECTION_HPP
#define __HAS_CRITICAL_SECTION_HPP

#include <vector>
#include <vector>

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

    class CriticalSection {
    public:
        int res_id;
        double duration;
        std::vector<CriticalSection> nested;
        bool is_nested;
    
        CriticalSection(int rid, double dur, bool nest = false) : 
            res_id(rid), duration(dur), is_nested(nest) {}
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
    };

}
#endif // __HAS_CRITICAL_SECTION_HPP

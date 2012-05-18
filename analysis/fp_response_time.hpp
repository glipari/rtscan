#ifndef __FP_RESP_TIME_HPP__
#define __FP_RESP_TIME_HPP__

#include <algorithm>
#include <sstream>
#include <iostream>

#include <models/fp_task.hpp>
#include <analysis/task_utility.hpp>

namespace Scan {
    template <typename Iter, typename Prop>
    void sort_by_increasing(Iter b, Iter e, Prop f) 
    {
        typedef typename std::iterator_traits<Iter>::value_type Type;
        std::sort(b, e, [f](Type x, Type y) {
                return (x.*f)() < (y.*f)();
            });
    }

    template <typename Iter, typename Prop>
    void sort_by_decreasing(Iter b, Iter e, Prop f) 
    {
        typedef typename std::iterator_traits<Iter>::value_type Type;
        std::sort(b, e, [f](Type x, Type y) {
                return (x.*f)() > (y.*f)();
            });
    }
 
    template <typename Iter>
    void assign_rm_priorities(Iter b, Iter e) 
    {
        typedef typename std::iterator_traits<Iter>::reference Ref;
        typedef typename std::iterator_traits<Iter>::value_type Type;
        
        sort_by_increasing(b, e, &Type::get_period);      
        int k=0;
        for_each(b, e, [&k](Ref a) { k++; });
        for_each(b, e, [&k](Ref a) {
                a.set_priority(k); --k; 
            });
    }

    template <typename Iter>
    void assign_dm_priorities(Iter b, Iter e) 
    {
        typedef typename std::iterator_traits<Iter>::reference Ref;
        typedef typename std::iterator_traits<Iter>::value_type Type;

        sort_by_increasing(b, e, &Type::get_dline);        
        
        int k=0;
        for_each(b, e, [&k](Ref a) { k++; });
        for_each(b, e, [&k](Ref a) {
                a.set_priority(k); --k; 
            });
    }

    DECL_EXC(RespTimeTooLarge);

    template<class T>
    std::string toString(const T& d) 
    {
        std::stringstream ss;
        ss << d;
        return ss.str();
    }

    /** 
        Computes the workload of a set of tasks in interval [t1, t2)
    */
    template<typename Iter>
    double workload(Iter first, Iter end, double t1, double t2)
    {
        typedef typename std::iterator_traits<Iter>::value_type Type;
        double r = 0;
        for_each(first, end, [&r, t1, t2](Type i) {
                r += get_num_arrived_instances(i, t1, t2) *
                    i.get_wcet();
            });
        return r;
    }
    
    /** 
        Computes the worst case response time of the last task in the
        container. The assumption is that the container is ordered by
        decreasing priority
     */
    template<typename Iter>
    double resp_time(Iter highest, Iter end, double bound) 
    {
        typedef typename std::iterator_traits<Iter>::value_type Type;

        double r = 0;            // rt, used for the internal loop
        double r_old = 0;        // used for the internal loop
        double r_latest = 0;     // the greatest rt among all instances
        int k = 0;               // instance number
        Iter task = end-1;       // task under analysis
                                 
        // starting point: r^0
        for_each(highest, end, [&r](Type a) {
                r += a.get_wcet();
            });
                        
        do {
            k++;   
            while (r_old < r) {               
                r_old = r; 
                r = k * task->get_wcet() + workload(highest, task, 0, r_old);
                if (r > bound) throw RespTimeTooLarge(toString(r));
            }
            r_latest = std::max(r_latest, r - (k-1) * task->get_period());
            if (r > k * task->get_period()) r+=task->get_wcet();
        } while (r > k * task->get_period());        
        return r_latest;
    }

    template<class Iter>
    void align_critical_instant(Iter b, Iter e)
    {
        typedef typename std::iterator_traits<Iter>::reference Ref;
        for_each(b, e, [](Ref x) {
                x.set_offset(0);
            });
    }
}
#endif

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <cmath>
#include <boost/math/common_factor.hpp>
#include <common/exceptions.hpp>

namespace Scan {
    /**
       Computes the hyperperiod
     */
    template<class Iter>
    int compute_hyperperiod(Iter b, Iter e, int n)
    {
        int l = 1;
        int k=0;
        for (Iter i = b; i!=e && k<n; ++i, ++k) {
            l =  boost::math::lcm<int>(l, i->get_period());
        }
        return l;
    }

    /** 
        Returns the first arrival time after t (it always returns
        a number > t). 

        Requires functions get_period() and get_offset().
    */
    template <class T>
    double get_next_arrival(const T &tsk, double t)
    {
        double n = ceil((t-tsk.get_offset())/tsk.get_period()) * 
            tsk.get_period() + tsk.get_offset();

        if (n == t) return n + tsk.get_period();
        else return n;
    }

    /** 
        Returns the first arrival time after or at t (can return t). 

        Requires functions get_period() and get_offset().
    */
    template <class T>
    double get_next_arrival_eq(const T &tsk, double t)
    {
        double n = ceil((t-tsk.get_offset())/tsk.get_period()) * 
            tsk.get_period() + tsk.get_offset();
        return n;
    }
        
    /** 
        Returns the arrival time before or at t

        Requires functions get_period() and get_offset().
    */ 
    template <class T>
    double get_prev_arrival(const T &tsk, double t) 
    {
        double p = floor((t-tsk.get_offset())/tsk.get_period()) * 
            tsk.get_period() + tsk.get_offset();
        return p;
    }

    /**
       Returns the next deadline after t (it always returns a number > t)
           
       Requires functions get_period(), get_offset() and get_dline().   
    */
    template <class T>
    double get_next_deadline(const T &tsk, double t)
    {
        double n = get_prev_arrival(tsk, t) + tsk.get_dline();
        if (n <= t) return n + tsk.get_period();
        else return n;
    }

    /**
       Returns deadline before or at t (can return t itself)
           
       Requires functions get_period(), get_offset() and get_dline().   
    */
    template <class T>
    double get_deadline_before_at(const T &tsk, double t)
    {
        double n = get_prev_arrival(tsk, t) + tsk.get_dline();
        if (n > t) return n - tsk.get_period();
        else return n;
    }

    /**
       Returns the number of instances with arrival in [t1, t2)

       Requires functions get_period() and get_offset().
    */
    template <class T>
    int get_num_arrived_instances(const T &tsk, double t1, double t2)
    {
        if (t2 < t1) return 0;

        int n = 
            (int)ceil((t2-tsk.get_offset())/tsk.get_period()) - 
            (int)ceil((t1-tsk.get_offset())/tsk.get_period()); 
        return n;
    }

    /** 
        Returns the number of instances entirely contained in [t1,  t2] 
        (i.e. with arrival >= t1 and dline <= t2

        Requires functions get_period(), get_offset(), get_dline()
    */
    template <class T>
    int get_num_contained_instances(const T &tsk, double t1, double t2)
    {
        double x = get_next_arrival_eq(tsk, t1);
        double y = get_deadline_before_at(tsk, t2);
        if (y <= x) return 0;
        else return floor((y-x-tsk.get_dline())/tsk.get_period()) + 1;  
    }

    DECL_EXC(NonMonotonicFunction);
    DECL_EXC(FixPointDoesNotConverge);

    /**
       Computes the first fixed point of a monotonic function,
       starting from point start. The function is expected to be
       monotonically non-increasing or monotonically
       non-decreasing. The function must take a double and return
       a double.

       If the function is non monotonic, the NonMonotonicFunction
       exception is thrown; if the function does not converge
       before maxvalue, the function throws
       FixPointDoesNotConverge exception.
    */
    template <class F>
    double fix_point(F f, double start, double maxvalue = 1e15)
    {
        int sign;
        double old = start;   // 0
        start = f(start);     // -3

        old = start;          // -3
        start = f(old);       // -1.5

        if (start < old) sign = -1;  
        else if (start == old) return start;
        else if (start > old) sign = +1;  // sign = 1
            
        while ( ((old - start)*sign < 0) && fabs(start) < maxvalue ) {
            old = start;
            start = f(old);
        }
        if ((old-start)*sign > 0) 
            throw NonMonotonicFunction("Function is not monotonic");
        if (fabs(start) >= maxvalue) 
            throw FixPointDoesNotConverge("Function does not converge");
        else if (old == start) return start;
        else 
            std::cerr << "fix_point: Should never reach this point!" << std::endl;
        return 0;
    }
    
    /**
      Computes all deadlines in the hyperperiod.
    */
    template<class Iter>
    std::vector<int> compute_all_deadlines(Iter a, Iter b)
    {
        std::vector<int> dl;
        int h = compute_hyperperiod(a, b, b-a);
        for (Iter p = a; p!=b; ++p) {
            int d = get_next_deadline(*p, 0);
            while (d <= h) {
                dl.push_back(d);
                d = get_next_deadline(*p, d);
            }
        }
        std::sort(dl.begin(), dl.end());
        std::unique(dl.begin(), dl.end());
        return dl;
    }
}



#endif

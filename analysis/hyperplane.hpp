#ifndef __HYPERPLANE_HPP__
#define __HYPERPLANE_HPP__

#include <vector>
#include <cmath>
#include <ostream>
#include <algorithm>
#include <analysis/constraint_space.hpp>
#include <models/task.hpp>

namespace Scan {
    
    inline double compute_coeff(double t, int p)
    {
        return std::ceil(t/p);
    }

    // first try: Compute all points
    template <class Iter>
    std::vector<int> compute_all_points(Iter b, Iter e, double d)
    {
        std::vector<int> points;
        for (Iter i=b; i!=e; i++) {
            int k=1; 
            while (k * i->get_period() < d) {
                points.push_back(k * i->get_period()); k++;
            }
        }
        points.push_back(d);
        //sort(points.begin(), points.end());
        return points;
    }

    template <class ReverseIter>
    std::vector<int> compute_points(ReverseIter rbeg, ReverseIter rend, double d)
    {
        std::vector<int> points;
        if (rend == rbeg) {
            if (d != 0) points.push_back(d);
        }
        else {
            int t = int(std::floor(d / rbeg->get_period())) * rbeg->get_period();
            std::vector<int> a = compute_points(rbeg+1, rend, d);
            std::vector<int> b = compute_points(rbeg+1, rend, t);
            
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(points));
        }
        return points;
    }

    std::vector<double> compute_points(const std::vector<Task> &tasks, 
                                       int i, double d);

    ConjunctionSet create_space(const std::vector<Task> &tasks);

}

#endif

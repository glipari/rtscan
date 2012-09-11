#ifndef __HYPERPLANE_HPP__
#define __HYPERPLANE_HPP__

#include <vector>
#include <cmath>
#include <ostream>
#include <analysis/constraint_space.hpp>
#include <models/task.hpp>

namespace Scan {
    
    inline double compute_coeff(double t, int p)
    {
        return std::ceil(t/p);
    }

    template <class Iter>
    std::vector<double> compute_points(Iter b, Iter e, double d)
    {
        std::vector<double> points;
        // first try: all points
        for (Iter i=b; i !=e; i++) {
            int k=1; 
            while (k * i->get_period() < d) {
                points.push_back(k * i->get_period()); k++;
            }
        }
        points.push_back(d);
        return points;
    }

    std::vector<double> compute_points(const std::vector<Task> &tasks, 
                                       int i, double d);

}

#endif

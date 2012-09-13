#include <cassert>
#include <algorithm>
#include "hyperplane.hpp"

namespace Scan {

    using namespace std;

    vector<double> compute_points(const vector<Task> &tasks, int i, double d)
    {
        vector<double> points;
        if (i == -1) {
            points.push_back(d);
        }
        else {
            double t = std::floor(d / tasks[i].get_period()) * tasks[i].get_period();
            vector<double> a = compute_points(tasks, i-1, d);
            vector<double> b = compute_points(tasks, i-1, t);
            
            sort(a.begin(), a.end());
            sort(b.begin(), b.end());
            set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(points));
        }
        return points;
    }

    ConjunctionSet create_space(const vector<Task> &tasks)
    {
        Conjunction nn = non_negative_space(tasks.size());
        ConjunctionSet space(tasks.size());
        space.add_constraint(nn);
        for (unsigned i=0; i<tasks.size(); i++) {
            vector<double> points = compute_points(tasks, i, tasks[i].get_dline());
            DisjunctionSet ds(tasks.size());
            for (auto t : points) {
                vector<double> row;
                for (unsigned k=0; k<i; k++) {
                    row.push_back(compute_coeff(t, tasks[k].get_period()));
                }
                row.push_back(1);
                for (unsigned k=i+1; k<tasks.size(); k++) {
                    row.push_back(0);
                }
                Plane plane(row, Plane::lte, t);
                ds.add_constraint(plane);
            }
            space.add_constraint(ds);
        }
        return space;
    }
}

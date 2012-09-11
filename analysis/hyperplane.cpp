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

    conjunct_space_t create_space(const vector<Task> &tasks)
    {
        conjunct_space_t nn = non_negative_space(tasks.size());
        for (unsigned i=0; i<tasks.size(); i++) {
            vector<double> points = compute_points(tasks, i, tasks[i].get_dline());
            disjunct_space_t ds;
            for (auto t : points) {
                vector<double> row;
                for (unsigned k=0; k<i; k++) {
                    row.push_back(compute_coeff(t, tasks[k].get_period()));
                }
                row.push_back(1);
                for (unsigned k=i+1; k<tasks.size(); k++) {
                    row.push_back(0);
                }
                plane_t plane(row, plane_t::lte, t);
                ds.add_constraint(plane);
            }
            nn.add_constraint(ds);
        }
        return nn;
    }
}

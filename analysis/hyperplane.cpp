#include <cassert>
#include <algorithm>
#include "hyperplane.hpp"

namespace Scan {

    using namespace std;

    vector<double> compute_points(const vector<Task> &tasks, int i, double d)
    {
        vector<double> points;
        if (i == 0) {
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


}

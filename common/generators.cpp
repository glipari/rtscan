#include "generators.hpp"
#include <random>
#include <cmath>

namespace Scan {
    /**
       UUniFast Algorithm by Enrico Bini
     */
    std::vector<double> UUniFast(int n, double U_target)
    {
        std::mt19937 rng;
        std::uniform_real_distribution<double> dist(0,1);
        
        std::vector<double> v; 
        double sum = U_target;
        for (int i=1; i<=n-1; i++) {
            
            double next = sum * pow(dist(rng), 1.0/double(n-i));
            v.push_back(sum - next);
            sum = next;
        }
        v.push_back(sum);
        return v;
    }
}

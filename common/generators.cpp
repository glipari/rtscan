#include <cmath>

#include "generators.hpp"
#include "sequence.hpp"

namespace Scan {
    using namespace std;
    
    rndgen_t *GeneratorSingleton::rng = 0;

    rndgen_t &GeneratorSingleton::get() {
        if (rng == 0) rng = new rndgen_t();
        return *rng;
    }


    void GeneratorSingleton::init(int seed) {
        delete rng;
        rng = new rndgen_t(seed);
    }


    /**
       UUniFast Algorithm by Enrico Bini
     */
    std::vector<double> UUniFast(int n, double U_target, rndgen_t &rng)
    {
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

    void __gen(unsigned g, const std::vector< std::pair<int, int> >& limits, vector< vector<int> > &elems, vector<int> point)
    {
        if (g == limits.size()) {
            elems.push_back(point);
            return;
        }
        else {
            for (int i=limits[g].first; i<limits[g].second+1; ++i) {
                vector<int> p2 = point;
                p2.push_back(i);
                __gen(g+1, limits, elems, p2);
            } 
        }
    }

    std::vector< std::vector<int> > cartesian_product(const std::vector< std::pair<int, int> >& limits)
    {
        vector< vector<int> > elems;
        vector<int> point;
        unsigned g = 0;
        __gen (g, limits, elems, point);
        return elems;
    }

    std::vector< std::vector<int> > select_sum_vectors(const std::vector< std::vector<int> > & elems, int sum)
    {
        vector< vector<int> > good;
        
        select(elems.begin(), elems.end(), back_inserter(good), [sum](const vector<int> &x) {
                int y = 0;
                for (unsigned i=0; i<x.size(); i++) y+=x[i];
                return y == sum;
            });

        return good;
    }
    
    std::vector<int> random_bins(const std::vector< std::pair<int, int> >& limits, int sum, rndgen_t &rng)
    {
        vector< vector<int> > good = select_sum_vectors(cartesian_product(limits), sum);
        
        std::uniform_int_distribution<int> dist(1, good.size());
        
        return good[dist(rng)-1];
    }
}

#ifndef __LG_MULTI_BAKER_HPP__
#define __LG_MULTI_BAKER_HPP__

#include <vector>
#include <models/task.hpp>
#include <random>

namespace Scan {
    typedef std::mt19937 rndgen_t;

#define RNDGEN() GeneratorSingleton::get()


    class GeneratorSingleton {
        static rndgen_t rng;
    public:
        static rndgen_t &get();
    };

    std::vector<double> UUniFast(int nproc, double U_target, rndgen_t &rng = RNDGEN());

    std::vector<int> random_bins(const std::vector< std::pair<int, int> >& limits, int sum, rndgen_t &rng = RNDGEN());
    std::vector< std::vector<int> > cartesian_product(const std::vector< std::pair<int, int> >& limits);
    std::vector< std::vector<int> > select_sum_vectors(const std::vector< std::vector<int> > & elems, int sum);
}

#endif

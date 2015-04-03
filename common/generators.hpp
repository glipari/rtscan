#ifndef __LG_MULTI_BAKER_HPP__
#define __LG_MULTI_BAKER_HPP__

#include <vector>
#include <models/task.hpp>
#include <random>

namespace Scan {
    typedef std::mt19937 rndgen_t;

#define RNDGEN() GeneratorSingleton::get()

    /**
       This Singleton class provides a single random generator,
       accessible by all objects with the macro RNDGEN().

       It can be initialized with a certain seed with the init()
       member function. init() can be called at anytime to reset the
       seed.
     */
    class GeneratorSingleton {
        static rndgen_t *rng;
    public:
        static rndgen_t &get();

        // set the seed
        static void init(int seed);
    };

    /** 
        The UUniFast algorithm by Enrico Bini.

        @param ntasks: number of tasks to generate
        @param U_target: the target utilization
        @param rng: random generator (by default, the standard one)
    */
    std::vector<double> UUniFast(int nproc, double U_target, rndgen_t &rng = RNDGEN());
    
    /**
       Returns a vector of integer number. Every number must be
       between the corresponding limits (as specified by the vector
       "limits"), and their sum must be equal to "sum".

       Example: limits = { {1,4}, {2,3} } and sum = 6, may return the
       following two vectors: {3, 3} or {2, 4} with equal probability.

       Basically, this is similar to UUniFast, but for integral
       numbers. It works by enumerating all feasible vectors, and then
       choosing one at random with uniform distribution.

       @param limits: a vector of pairs, each one specifies a minimum
       and a maximum for each bin.

       @param sum: the desired sum of all vector elements

       @param: a random number generator (by default the global one).
     */
    std::vector<int> random_bins(const std::vector< std::pair<int, int> >& limits, int sum, rndgen_t &rng = RNDGEN());

    /**
       This is an helper function for random_bins(). It takes as input
       a vector of n pairs <min,max>. For each element of the vector,
       <min,max> represents the interval of values of the i-th
       elements in the result.

       Then it produces a set of vectors of n integers, with all
       possible combination of values. For example, if we pass <2,3>
       and <4,5> as parameters, we obtain : 

       <2, 4>, <2, 5>, <3, 4>, <3, 5>. 
     */
    std::vector< std::vector<int> > cartesian_product(const std::vector< std::pair<int, int> >& limits);

    /**
       This is a helper function for random_bins(). It takes a set of
       vectors of n elements, and returns a subset in which all
       vectors have the sum of the elements equal to sum.
     */
    std::vector< std::vector<int> > select_sum_vectors(const std::vector< std::vector<int> > & elems, int sum);


    /// Exception: if select_sum_vectors() does not provide any
    /// feasible solution, the exception is thrown.
    DECL_EXC(UnfeasibleBins);
}

#endif

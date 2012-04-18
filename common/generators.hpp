#ifndef __LG_MULTI_BAKER_HPP__
#define __LG_MULTI_BAKER_HPP__

#include <vector>
#include <models/task.hpp>

namespace Scan {
    std::vector<double> UUniFast(int nproc, double U_target);



    // - second, generate the periods OR the computation times, according to some 
    // rule (see period generator or wcet generation)

    // - third, compute the other WCET or period
    
    // - generate deadline
    
    // - check schedulability

    // - continue until a certain point
    class TaskGen_MultiBaker {
        std::vector<Task> tset;
    public:
        
    };
}

#endif

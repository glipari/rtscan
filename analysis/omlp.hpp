#ifndef __OMLP_HPP__
#define __OMLP_HPP__

#include <analysis/fmlp.hpp>

namespace Scan {

    class OMLPAnalysis {
        int num_proc;
        std::vector<TaskRes> tset;
        std::vector<Resource> rset;
    public:
        template<class TaskIter, class ResIter>
        OMLPAnalysis(int nproc, TaskIter tset_begin, TaskIter tset_end, ResIter res_begin, ResIter res_end) :
            num_proc(nproc),
            tset(tset_begin, tset_end),
            rset(res_begin, res_end)
            {
            }

        double blocking_time_global(const TaskRes &task);
    };
}

#endif

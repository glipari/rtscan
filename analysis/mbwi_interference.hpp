#ifndef __MBWI_INTEFERENCE_HPP__
#define __MBWI_INTEFERENCE_HPP__

#include <vector>
#include <models/taskres.hpp>

namespace Scan {
    double mbwi_compute_interference(const std::vector<TaskRes>& taskset, 
                                     const std::vector<TaskRes>& blocked_tasks, 
                                     const std::vector<int>& blocked_res, 
                                     CSSet::const_iterator csset_begin,
                                     CSSet::const_iterator csset_end);

    double mbwi_compute_interference(const TaskRes& task,
                                     std::vector<TaskRes> taskset);

    std::vector<TaskRes> compute_gamma(const std::vector<TaskRes>& taskset, int r_k);

}

#endif

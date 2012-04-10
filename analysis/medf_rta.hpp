#ifndef __MEDF_RTA_HPP__
#define __MEDF_RTA_HPP__

#include <vector>
#include <models/task.hpp>

namespace Scan {
    bool rta_compl_edf(int n_proc, std::vector<Task> &task);
}

#endif

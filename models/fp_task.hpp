#ifndef __FPTASK_HPP__
#define __FPTASK_HPP__

#include <models/task_ex.hpp>
#include <models/haspriority.hpp>
#include <common/property.hpp>

namespace Scan {
    typedef TaskEx<HasPriority> FPTask;    

//    std::ostream & operator<<(std::ostream &o, FPTask &t); 
}

#endif

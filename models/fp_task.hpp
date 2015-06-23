#ifndef __FPTASK_HPP__
#define __FPTASK_HPP__

#include <models/task_ex.hpp>
#include <models/has_priority.hpp>
#include <common/property.hpp>
#include <boost/shared_ptr.hpp>

namespace Scan {
    /// A task with a fixed priority
    typedef TaskEx<HasPriority> FPTask;

    /// A FP task pointer
    typedef boost::shared_ptr<FPTask> FPTask_ptr;

    std::ostream & operator<<(std::ostream &o, const FPTask &t); 
    std::ostream & operator<<(std::ostream &o, const FPTask_ptr &t); 
}

#endif

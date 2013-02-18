#ifndef __FPTASK_HPP__
#define __FPTASK_HPP__

#include <models/task_ex.hpp>
#include <models/haspriority.hpp>
#include <common/property.hpp>
#include <boost/shared_ptr.hpp>

namespace Scan {
    typedef TaskEx<HasPriority> FPTask;    
    typedef boost::shared_ptr<FPTask> FPTask_ptr;


    std::ostream & operator<<(std::ostream &o, const FPTask &t); 
    std::ostream & operator<<(std::ostream &o, const FPTask_ptr &t); 
}

#endif

#ifndef __TASKRES_HPP__
#define __TASKRES_HPP__

#include <models/task_ex.hpp>
#include <models/has_critical_section.hpp>
#include <models/has_priority.hpp>

namespace Scan {
    typedef TaskEx<HasCriticalSection> TaskRes;
    typedef TaskEx<HasCriticalSection,HasPriority> FPTaskRes;

    // TODO explicit specialization to allow conversion 
    // between TaskRes and FPTaskRes
}

#endif

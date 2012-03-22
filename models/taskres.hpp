#ifndef __TASKRES_HPP__
#define __TASKRES_HPP__

namespace Scan {
    typedef TaskEx<HasCriticalSection> TaskRes;
    typedef TaskEx<HasCriticalSection,HasPriority> FPTaskRes;

    // TODO explicit specialization to allow conversion 
    // between TaskRes and FPTaskRes
}

#endif

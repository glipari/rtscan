#ifndef __TASK_EX_HPP__
#define __TASK_EX_HPP__

#include <models/task.hpp>

namespace Scan {
    template <class... Properties>
    class TaskEx : public Task, public Properties... {
    public:
        TaskEx() : Task() {}
        TaskEx(double wcet, double dline, int period, int offset=0, double jitter=0) throw(IllegalValue) : 
            Task(wcet, dline, period, offset, jitter) {}
        TaskEx(double wcet, double dline, int period, int offset, double jitter, int ppos) throw(IllegalValue) : 
            Task(wcet, dline, period, offset, jitter, ppos) {}
        TaskEx(const Task &other) : Task(other) {}

    };
}

#endif


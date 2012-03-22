#ifndef __TASK_PRIORITY_HPP__
#define __TASK_PRIORITY_HPP__

class HasPriority {
    int priority;
public:
    HasPriority() : priority(0) {}
    int get_priority() { return priority; }
    void set_priority(int p) { priority = p; }
};

#endif

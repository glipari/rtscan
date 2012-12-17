#ifndef __HAS_PROCESSOR_HPP__
#define __HAS_PROCESSOR_HPP__

namespace Scan {
    /**
       Models a task allocated on a certain processor.
       
       the processor variable is the processor index (an integer) on
       which the task is allocated; If -1, the task has not been
       allocated yet. If >=0, it represents a real processor.

       It is useful for partitioning algorithms and on distributed
       systems.
     */
    class HasProcessor {
        int processor;
    public:
        HasProcessor() : processor(-1) {}
        inline int get_proc() const { return processor; }
        inline void set_proc(int p) { processor = p; }
    };
    
    /**
       True if a task is allocated on a certain processor. 
     */
    class AllocatedPredicate {
        int proc;
    public:
        inline AllocatedPredicate(int p) : proc(p) {}
        inline bool operator()(const HasProcessor &task) { 
            return task.get_proc() == proc;
        }
    };
}

#endif

#ifndef __TASK_MODEL_HPP__
#define __TASK_MODEL_HPP__

#include <vector>
#include <string>

namespace Scan {
    namespace TaskModel {
        struct prop_struct {
            std::string name;
            double value;
        };
        
        struct task_struct {
            std::string name;
            std::vector< prop_struct > properties;
        };
    }
}

#endif // 

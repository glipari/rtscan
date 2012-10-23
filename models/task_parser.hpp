#ifndef TASK_PARSER_HPP
#define TASK_PARSER_HPP

#include <common/property_parser.hpp>
#include <models/task.hpp>
#include <models/fp_task.hpp>

namespace Scan {
    class TaskVisitor: public GenPropertyVisitor {
    public:
        TaskVisitor();
        Task create_task();
    };
    Task create_task(const PropertyList &p);

    class FPTaskVisitor : public TaskVisitor {
    public:
        FPTaskVisitor();
        FPTask create_task();
    };

    FPTask create_fp_task(const PropertyList &p);
    


}

#endif

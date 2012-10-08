#include <models/task_parser.hpp>

namespace Scan {
    TaskVisitor::TaskVisitor() 
    {
        add_double_parameter({"wcet", "c", "C"}, 0, true);
        add_double_parameter({"jitter", "j", "J"}, 0);
        add_double_parameter({"dline", "d", "D"}, 0);
        add_int_parameter({"period", "T", "p"}, 0, true);
        add_int_parameter({"offset", "o", "off"}, 0);
    }

    Task TaskVisitor::create_task() {
        if (!check_mandatory()) THROW_EXC(IllegalValue, "Mandatory parameter is missing");
        
        Task t(d_values["wcet"], 
               d_values["dline"],
               i_values["period"], 
               d_values["jitter"], 
               d_values["offset"]);
        t.set_name(name);
        return t;
    }

    Task create_task(const PropertyList &p)
    {
        if (p.type != "task") THROW_EXC(IncorrectType, "Expecting a task type");
        TaskVisitor vis;
        vis(p);
        return vis.create_task();
    }


    FPTaskVisitor::FPTaskVisitor()
    {
        add_int_parameter({"priority", "prio"}, 1, true);
        
    }
    FPTask FPTaskVisitor::create_task()
    {
        if (!check_mandatory()) THROW_EXC(IllegalValue, "Mandatory parameter is missing");
        
        FPTask t(d_values["wcet"], 
                 d_values["dline"],
                 i_values["period"], 
                 d_values["jitter"], 
                 d_values["offset"]);
        t.set_priority(i_values["priority"]);
        t.set_name(name);
        return t;
    }
    

    FPTask create_fp_task(const PropertyList &p)
    {
        if (p.type != "task") THROW_EXC(IncorrectType, "Expecting a task type");
        FPTaskVisitor vis;
        vis(p);
        return vis.create_task();
    }

}

#include <models/task_parser.hpp>
#include <climits>

namespace Scan {
    TaskVisitor::TaskVisitor() 
    {
        add_double_parameter({"wcet", "c", "C"}, 0, true);
        add_double_parameter({"jitter", "j", "J"}, 0);
        add_double_parameter({"dline", "d", "D", "deadline"}, -1);
        add_int_parameter({"period", "T", "p"}, 0); 
	//here period T is set as an optional field
        add_int_parameter({"offset", "o", "off"}, 0);
	add_string_parameter({"sched", "s", "sch"}, "fp"); //, true);
	add_int_parameter({"node", "n", "node"}, 1);
	add_int_parameter({"pipelinepos", "p", "pos"}, -1);
    }

    Task TaskVisitor::create_task() {
        if (!check_mandatory()) THROW_EXC(IllegalValue, "Mandatory parameter is missing");
        if (d_values["dline"] < 0.0) 
            d_values["dline"] = i_values["period"];
        
        Task t(d_values["wcet"], 
               d_values["dline"],
               i_values["period"], 
               d_values["jitter"], 
               d_values["offset"]);
        t.set_sched(s_values["sched"]);
        t.set_node(i_values["node"]);
        t.set_pipeline_pos(i_values["pipelinepos"]);
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


    FPTaskVisitor::FPTaskVisitor() : TaskVisitor()
    {
        add_int_parameter({"priority", "prio"}, 1, true);
    }

    FPTask FPTaskVisitor::create_task()
    {
        if (!check_mandatory()) 
            THROW_EXC(IllegalValue, "Mandatory parameter is missing");
	setup_defaults();
        if (d_values["dline"] < 0.0) 
            d_values["dline"] = i_values["period"];
        FPTask t(d_values["wcet"], 
                 d_values["dline"],
                 i_values["period"], 
                 d_values["offset"],
                 d_values["jitter"]);
	//	 i_values["pipelinepos"]);
        t.set_priority(i_values["priority"]);
        t.set_sched(s_values["sched"]);
        t.set_node(i_values["node"]);
        t.set_pipeline_pos(i_values["pipelinepos"]);
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

    FPTask_ptr FPTaskVisitor::create_task_ptr()
    {
        if (!check_mandatory()) 
            THROW_EXC(IllegalValue, "Mandatory parameter is missing");
	setup_defaults();
        if (d_values["dline"] < 0.0) 
            d_values["dline"] = i_values["period"];
        FPTask_ptr t ( new FPTask(d_values["wcet"], 
                 d_values["dline"],
                 i_values["period"], 
                 d_values["offset"],
                 d_values["jitter"]) );
	//	 i_values["pipelinepos"]);
        t->set_priority(i_values["priority"]);
        t->set_sched(s_values["sched"]);
        t->set_node(i_values["node"]);
        t->set_pipeline_pos(i_values["pipelinepos"]);
        t->set_name(name);
        return t;
    }
    

    FPTask_ptr create_fp_task_ptr(const PropertyList &p)
    {
        if (p.type != "task") THROW_EXC(IncorrectType, "Expecting a task type");
        FPTaskVisitor vis;
        vis(p);
        return vis.create_task_ptr();
    }
}

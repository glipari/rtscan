#include <sstream>
#include <cmath>
#include <map>
#include <vector>
#include <string>

#include <models/task.hpp>
#include <common/property_parser.hpp>

using namespace std;

namespace Scan {
    template<>
    int HasUniqueId<Task>::counter = 0;

    void Task::init_name_()
    {
        stringstream nn;
        nn << "AnonymousTask"; 
        name = string(nn.str());        
    }

    Task::Task() : period(0), wcet(0), dline(0)
    {
        init_name_();
    }

    Task::Task(const string &n) : name(n), period(0), wcet(0), dline(0)
    {   
    }

    Task::Task(double w, double d, int p, int off, double jit) 
        throw(IllegalValue) : period(p), wcet(w), dline(d), offset(off), jitter(jit)
    {
        if (w > p || w < 0 || p < 0 || d < 0) {
 
            stringstream err;
            err << "cannot create task with parameters w: " 
                << w << " p : " << p << " d : " << d;
            
            throw IllegalValue(err.str());
        }
        init_name_();
    }
    
    Task::Task(const Task &t) : HasUniqueId(t), name(t.name), period(t.period), wcet(t.wcet), dline(t.dline), offset(t.offset), jitter(t.jitter), sched(t.sched), node(t.node), pipeline_pos(t.pipeline_pos), pipeline_tag(t.pipeline_tag)
    {
    }

    Task & Task::operator=(const Task &t)
    {
        HasUniqueId::operator=(t);
        name = t.name;
        period = t.period;
        dline = t.dline;
        wcet = t.wcet;
        offset = t.offset;
        jitter = t.jitter;
        sched = t.sched;
        node = t.node;
        pipeline_pos = t.pipeline_pos;
        pipeline_tag = t.pipeline_tag;

        return *this;
    }

    int Task::get_period() const
    {
        return period;
    }
    
    double Task::get_wcet() const
    {
        return wcet;
    }

    double Task::get_dline() const
    {
        return dline;
    }

    void Task::set_period(int t) throw(IllegalValue)
    {
        if (wcet > t || t < 0) {
            stringstream err;
            err << "cannot set period p: " << t << " with wcet w: " << wcet;
            
            throw IllegalValue(err.str());
        }
        period = t;
    }

    void Task::set_wcet(double w) throw(IllegalValue)
    {
        if (w > period || w < 0) {
            stringstream err;
            err << "cannot set wcet w: " << w << " with period p: " 
                << period;             
            throw IllegalValue(err.str());
        }
        wcet = w;
    }

    void Task::set_dline(double d) throw(IllegalValue)
    {
        if (d < 0) {
            stringstream err;
            err << "cannot set dline d: " << d;
            
            throw IllegalValue(err.str());
        }
        dline = d;
    }

    std::ostream & operator<<(std::ostream &o, const Task &t)
    {
        o << t.get_wcet() << "  " << t.get_dline() << "  " << t.get_period(); 
        return o;
    }

    std::istream & operator>>(std::istream &i, Task &t) throw(IllegalValue)
    {
        double w, d;
        int p;
        i >> w >> d >> p;

        t.set_period(p); t.set_dline(d); t.set_wcet(w); 
        return i;
    }



    void Task::configure_visitor(GenPropertyVisitor &pv)
    {
        pv.add_double_parameter({"wcet", "c", "C"}, 0, true);
        pv.add_double_parameter({"jitter", "j", "J"}, 0);
        pv.add_double_parameter({"dline", "d", "D", "deadline"}, -1);
        pv.add_int_parameter({"period", "T", "p"}, 0); 
        //here period T is set as an optional field
        pv.add_int_parameter({"offset", "o", "off"}, 0);
        pv.add_string_parameter({"sched", "s", "sch"}, "fp"); 
        pv.add_int_parameter({"node", "n", "node"}, 1);
        pv.add_int_parameter({"pipelinepos", "p", "pos"}, -1);
    }

    void Task::init_attributes(const GenPropertyVisitor &pv)
    {
        if (!pv.check_mandatory()) THROW_EXC(IllegalValue, 
                                             "Mandatory parameter is missing");

        double d = pv.get_dvalue("dline");
        
        if (d < 0.0) 
            d = pv.get_ivalue("period");

        // cout << "p: " << pv.get_ivalue("period") 
        //      << " d: " << pv.get_dvalue("dline") 
        //      << " w: " << pv.get_dvalue("wcet") 
        //      << " off: " << pv.get_ivalue("offset") << endl; 
            
        set_period(pv.get_ivalue("period"));
        set_dline(d);
        set_wcet(pv.get_dvalue("wcet")); 
              
        // set_jitter(pv.get_dvalue("jitter"));
        set_offset(pv.get_ivalue("offset"));

        // set_sched(pv.get_svalue("sched"));
        set_node(pv.get_ivalue("node"));
        set_pipeline_pos(pv.get_ivalue("pipelinepos"));
        set_name(pv.get_name());        
    }
    
    void Task::parse(const PropertyList &p)
    {
        if (p.type != "task") 
            THROW_EXC(IncorrectType, "Expecting a task type");

        GenPropertyVisitor vis;
        configure_visitor(vis); 
        vis(p);
        vis.setup_defaults();
        init_attributes(vis);
    }
}

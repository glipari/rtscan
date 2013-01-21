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

    Task::Task() : period(0), wcet(0), dline(0), pipeline_pos(0) 
    {
        init_name_();
    }

    Task::Task(const string &n) : name(n), period(0), wcet(0), dline(0), pipeline_pos(0) 
    {   
    }

    Task::Task(double w, double d, int p, int off, double jit) 
        throw(IllegalValue) : period(p), wcet(w), dline(d), offset(off), jitter(jit), pipeline_pos(0)
    {
/**
	Yes, this is dangerous ...
        if (w > p || w < 0 || p < 0 || d < 0) {
 */
        if (w < 0 || p < 0 || d < 0) {
            stringstream err;
            err << "cannot create task with parameters w: " 
                << w << " p : " << p << " d : " << d;
            
            throw IllegalValue(err.str());
        }
        init_name_();
    }
    
    Task::Task(double w, double d, int p, int off, double jit, int ppos) 
        throw(IllegalValue) : period(p), wcet(w), dline(d), offset(off), jitter(jit), pipeline_pos(ppos), HasUniqueId(ppos)
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

    using namespace std;
}

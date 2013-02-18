#include "fp_task.hpp"

namespace Scan {

	std::ostream & operator<<(std::ostream &o, const FPTask &t)
    {
        o << t.get_name() << ", node: " << t.get_node() + 1 << ", wcet: ";
        o << t.get_wcet() << ", dline: " << t.get_dline() << ", period: ";
        o << t.get_period() <<", sched: "<< t.get_sched() << ", prio: ";
        o << t.get_priority() ;
        return o;
    }

	std::ostream & operator<<(std::ostream &o, const FPTask_ptr &t)
    {
        o << t->get_name() << ", node: " << t->get_node() + 1 << ", wcet: ";
        o << t->get_wcet() << ", dline: " << t->get_dline() << ", period: ";
        o << t->get_period() <<", sched: "<< t->get_sched() << ", prio: ";
        o << t->get_priority() ;
        return o;
    }
}

#ifndef __TASK_PRIORITY_HPP__
#define __TASK_PRIORITY_HPP__

#include <common/property_parser.hpp>

namespace Scan {
class HasPriority {
    int priority;
protected:
    void configure_visitor(GenPropertyVisitor &pv) {
        pv.add_int_parameter({"priority", "prio"}, 1, true);
        std::cout << "configure called" << std::endl;
    }
    void init_attributes(GenPropertyVisitor &pv) {
        priority = pv.get_ivalue("priority");
        std::cout << "init called" << std::endl;
    }

public:
    HasPriority() : priority(0) {}
    int get_priority() const { return priority; }
    void set_priority(int p) { priority = p; }
};

}

#endif

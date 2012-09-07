#ifndef __TASK_MODEL_HPP__
#define __TASK_MODEL_HPP__

#include <vector>
#include <string>

#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

namespace Scan {
    namespace Model {
        struct resource {
            std::string name;
            int id;
            bool is_short;
        
            resource() : name(""), id(0), is_short(false) {}
        };

        struct cs_struct {
            int rid;
            double dur;
            std::vector< boost::recursive_wrapper<cs_struct> > nested;
        };
        
        struct task_struct {
            std::string name;
            double w;
            int p;
            double d;
            int off;
            std::vector< boost::recursive_wrapper<cs_struct> > nested;
            task_struct() : name(""), w(0), p(0), d(0), off(0) {}
        };

        typedef boost::variant<resource, task_struct> sys_elem; 
        typedef std::vector<sys_elem> sys_type;
    }
}


#endif // 

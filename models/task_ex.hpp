#ifndef __TASK_EX_HPP__
#define __TASK_EX_HPP__

#include <models/task.hpp>

namespace Scan {
    
    /**
       This template is used to extend the task model by including
       different properties.  The properties are additional classes
       which contain the corresponding attributes and accessors
       (get/set). See FPTask. 
     */
    template <class... Properties>
    class TaskEx : public Task, public Properties... {
        
    public:
        TaskEx() : Task() {}
        TaskEx(double wcet, double dline, int period, int offset=0, double jitter=0) throw(IllegalValue) : 
            Task(wcet, dline, period, offset, jitter) {}
        TaskEx(const Task &other) : Task(other) {}

        void parse(const PropertyList &p) {
            if (p.type != "task") THROW_EXC(IncorrectType, "Expecting a task type");
            GenPropertyVisitor vis;

            // add the task parameters to parse
            Task::configure_visitor(vis);

            /* The following needs to be explained: I want to call the
               member function configure_visitors() on all parent
               classes. Since it is not possible to iterate over
               variadic templates, I need to use one of the many
               obscure syntax tricks of C++11.  

               In this case: I define a (lambda) function that takes a
               variable number of arguments of any type and does
               nothing; its only task is to expand and evaluate its
               arguments.


               Then I build the expansion as argument list. This is
               done by "unpacking" Properties and calling the function
               on it. Since the return value of the member function is
               void, I use the comma operator with second operand 1 to
               obtain the value to pass to the anonymous function. And
               that's it!
            */ 
            [](...){ }(((Properties::configure_visitor(vis)), 1)... );

            // parse the properties
            vis(p);
            // set default value for unset properties
            vis.setup_defaults();

            // same as above but for initializing attributes
            [](...){ }(((Properties::init_attributes(vis)), 1)... );
        }
    };
}

#endif


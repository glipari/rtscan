#ifndef __TASK_H__
#define __TASK_H__

#include <iostream>
#include <string>
#include <memory>

#include <common/exceptions.hpp>
#include <models/has_unique_id.hpp>
#include <common/property.hpp>

namespace  Scan {
    /** 
        This class models a sporadic task or a periodic non-concrete
        task (also called offset-free task).
    */
    class Task : public HasUniqueId<Task> {
    private:
        std::string name;
        void init_name_();
    public: 
        /**
           Default constructor, all values init to 0. The name will be
           a number initialized to a increasing value
        */
        Task();

        /**
           Constructor, it is possible to set the task name by passing
           a constant string.
        */
        explicit Task(const std::string &n);

        /**
           Constructors with initializers for all variables.  This
           constructor checks the specification, i.e. wcet <= period
           and wcet <=dline.  dline can be greater or smaller than the
           period. In case the constraints are not verified, it throws
           an IllegalValue exception.
        */
        Task(double wcet, double dline, int period, int offset=0, double jitter=0) throw(IllegalValue);

        /// copy constructor
        Task(const Task &t);

        /// assignment operator
        Task & operator=(const Task &t);
        
        /// returns the task period
        int get_period() const;

        /// returns the worst-case execution time
        double get_wcet() const;

        /// returns the deadline
        double get_dline() const;

        /// returns the offset
        double get_offset() const { return offset; }

        /// returns the jitter
        double get_jitter() const { return jitter; }

        /// returns the task name
        std::string get_name() const { return name; }
        
        /// sets the task name
        void set_name(const std::string &n) { name = n; } 

        /// sets the period
        void set_period(int t) throw(IllegalValue); 

        /// sets the worst-case execution time
        void set_wcet(double w) throw(IllegalValue); 

        /// sets the deadline
        void set_dline(double d) throw(IllegalValue);     

        /// sets the offset
        void set_offset(int o) { offset = o; }     

        /// sets the offset
        void set_jitter(int j) { jitter = j; }     
            
    private:
        int period;
        double wcet;
        double dline;
        int offset;
        double jitter;
    };

    /** 
        Outputs the task parameters on a file. The output consists of
        the wcet value, the relative deadline, the task period, and
        the index on f the node on which the task is allocated,
        separated by spaces.
    */
    std::ostream & operator<<(std::ostream &o, const Task &t);

    /**
       Reads three numbers for the stream, the wcet, the relative
       deadline, the period, respectively. The function also checks that
       the values are legal, and throws an IllegalValue exception in
       case this is not true.
    */
    std::istream & operator>>(std::istream &i, Task &t) throw(IllegalValue);

    DECL_EXC(ValueAlreadySet);

    Task create_task(const PropertyList &p);
}

#endif

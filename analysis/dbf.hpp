#ifndef __DBF_H__
#define __DBF_H__

#include <iostream>
#include <vector>

#include <models/task.hpp>
#include <common/exceptions.hpp>

namespace Scan {

    DECL_EXC(LcmTooLarge);
    DECL_EXC(DbfNoSlope);

    class DemandBoundFunction;

    /**
       Sums two dbfs. This operator returns a new functor that contains the 
       sum of the two dbf. i.e. for each interval of time, sum the contribution
       of both dbf
     */
    DemandBoundFunction operator+(const DemandBoundFunction &d1, const DemandBoundFunction &d2);

    /**
       Computes a new dbf (i.e. a new function object) that is the least
       upper bound between two dbf.
     */
    DemandBoundFunction sup(const DemandBoundFunction &d1, const DemandBoundFunction &d2);

    /**
     * compares two dbfs
     */
    bool operator<=(const DemandBoundFunction &small, const DemandBoundFunction &big);

    /**
       Demand Bound Function definition. This is a functor that can be
       used to store the dbf of one or more tasks.
     */
    class DemandBoundFunction {
    public:
        /** Output format of a dbf. The gnuplot format is a format
            that can be read by the gnuplot program. pairlist is a
            list of pairs (time, value), on the same line. instants
            and values only prints the time instants at which the dbf
            changes value, and the corresponding values (input domain,
            and output domain).
         */
        typedef enum{gnuplot, pairlist, instants, values} print_format_t;

        /**
           This is an internal class that represents a point in the
           function. We store not only the instant and the value, but
           also the task id that produced that point.
         */
        class point_t {
        public:
			point_t() : first(0), second(0), tsk_tag_1(-1), trn_tag_1(-1), tsk_tag_2(-1), trn_tag_2(-1) {}
			point_t(double f, double s, int tsk_id_1 =-1, int trn_id_1=-1, int tsk_id_2 =-1, int trn_id_2=-1) :
				first(f), second(s), tsk_tag_1(tsk_id_1), trn_tag_1(trn_id_1), tsk_tag_2(tsk_id_2), trn_tag_2(trn_id_2) {}

            double first;
            double second;
            int tsk_tag_1;
            int trn_tag_1;
            int tsk_tag_2;
            int trn_tag_2;

            bool operator==(point_t &p) const {
                if (first == p.first && second == p.second) return true;
                else return false;
            }
        };

        std::vector<point_t> f;
        double interval;
        int period;

        print_format_t print;

    private:
        friend DemandBoundFunction operator+(const DemandBoundFunction &d1, const DemandBoundFunction &d2);
        friend DemandBoundFunction sup(const DemandBoundFunction &d1, const DemandBoundFunction &d2);
        friend std::ostream & operator<<(std::ostream &o, const DemandBoundFunction &d);
    public:
        /**
           Constructor, builds a null function (always returns 0 for
           any t).
         */
        DemandBoundFunction();
        /**
           Constructor of the dbf of a task
         */
        DemandBoundFunction(const Task &t);
        /**
           Copy constructor
         */
        DemandBoundFunction(const DemandBoundFunction& d);

        /**
           Sets the period. This function can only be called on a null
           dbf.
           
           @todo returns an error if it is not the null function.
        */
        void set_period(int period, double interval);

        /**
           Adds a point to the function. By default, the function is
           periodic. If the point is greater than the current period,
           an error should be returned.
           
           @todo return the error
        */
        void add_point(double t, double v, int tsk_1=-1, int trn_1=-1, int tsk_2=-1, int trn_2=-1);

        /// returns the value of the dbf in t
        double operator()(double t) const;

        /// Assignment operator
        DemandBoundFunction &operator=(const DemandBoundFunction &d);

        /**
           Returns a vector of doubles that contains all the instants
           when the dbf changes values
         */
        std::vector<double> get_points() const;
        
        /**
           Returns a vector of doubles that contains all the values
           assumed by the dbf
         */
        std::vector<double> get_values() const;

        /**
           Returns a vector of integers that contains all task ids
         */
        std::vector<int> get_tsk_id() const;

        /**
           Returns a vector of integers that contains all trans ids
         */
        std::vector<int> get_trn_id() const;

        /// returns the periodicity of the dbf
        int get_period() const { return period; }

        /// returns the lenght of the interval where the dbf is
        /// initally defined. It includes the period. Hence, the
        /// classical offset can be computed as
        /// get_interval()-get_period()
        double get_interval() const { return interval; }

        /// set the output format for operator <<. f must be one of
        /// the constants defined in enum print_format_t
        void set_output(print_format_t f) { print = f; }

        /// get the output format will be one of
        /// the constants defined in enum print_format_t
        print_format_t get_output() const { return print; }

        /**
           Checks "schedulability", i.e. that the demand bound function is 
           always below a line of slope alpha and passing by x = delta
         */
        bool check_sched(double alpha, double delta);

        /**
           compute the slope of this demand bound function,
           i.e. the value of alpha with a certain delta.
           @param[out] alpha: will contain the value of the slope
           @param[in] delta: the value of delta
           @return the critical instant (the first instant of the dbf that
			   touches the slope)
         */
        double compute_slope(double &alpha, double delta = 0.0);

    };
    
    /// output 
    std::ostream & operator<<(std::ostream &o, const DemandBoundFunction &d);

    bool check_gteq(const DemandBoundFunction big, const DemandBoundFunction small);
}

#endif


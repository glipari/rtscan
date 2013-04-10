#ifndef PPL_UTILS_HPP
#define PPL_UTILS_HPP
#include <ppl.hh>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/task_utility.hpp>
#include <common/exceptions.hpp>
#include <boost/math/common_factor.hpp>
#include <cmath>
#include <models/computational_resource.hpp>
#include <models/pipeline.hpp>
#include <models/pipeline_property_parser.hpp>
#include <map>
#include <utility>
#include <fstream>
#include <common/string_utils.hpp>
using namespace std;

namespace PPL = Parma_Polyhedra_Library;

namespace Scan {

/** 
    This class is used to read a .scan file and convert it into a set
    of tasks. The copy of smart pointers of each task is stored in
    its corresponding computational resource node and pipeline (if it
    is a pipeline task. This class applies the "visitor" pattern
*/
class SysVisitor : public boost::static_visitor<> {
public:
    /** A copy of all tasks in the system. */
    std::vector<Scan::FPTask_ptr> all_tasks;

    /** The set of computational resource nodes in the system. */
    Scan::ComputationalResource *node;

    /** The number of computational resource nodes in the system. */
    unsigned MAX_;

    /** The pipelines in the system. */
    vector<Scan::Pipeline> pipelines;

    /** This list contains all free variables in the system. */
    std::vector<std::string> vars_list;

    /**
         A copy of all tasks in the system. 
         This field is still kept for the compatibility with some early
         codes like driver/ppl/distributed.cpp, driver/ppl/sensitivity.cpp,
         and so on.     
    */
    std::vector<Scan::FPTask> v;

    /** By default, there is a single processor system. */
    SysVisitor(unsigned N = 1):MAX_(N) 
    {
	    node = new Scan::ComputationalResource[N];
    }

    ~SysVisitor()
    {
	    delete [] node;
    }

    /** To fetch a piece of property for a task or pipeline. */
    void operator()(const Scan::Property &p);

    /** 
        To analyze a property list and the result could be a whole 
        system, a pipeline, or a task.
    */
    void operator()(const Scan::PropertyList &pl);

    /** To parse the input file and fill the vars_list field. */
    void operator() (const std::string &vars_list_file);
};

/**
   This helper class simply joins together a powerset polyhedron and a
   the list of variable names. It is useful to operate symbolically on
   the pointset powerset.
 */
class ConstraintsSystem {
public:
    PPL::Pointset_Powerset<PPL::C_Polyhedron> poly;
    std::vector<std::string> vars;
    
    ConstraintsSystem();
    ConstraintsSystem(int n, PPL::Degenerate_Element kind=PPL::EMPTY);
   
    /**
        This method prints the generator of each disjoint polyhedron
        contained in poly into a set of files which later can be
        utilized by plotting tools like gnuplot, plotutils and python
        matplotlib easily.
    */ 
    void print_points(string fname);

    /**
       This function performs sensitivity analysis over a pointset
       powerset on one single variable, computing maximum and minimum
       values.
    */
    void do_sensitivity(const std::vector<FPTask> &tasks, 
                                        const std::string &var);
    void do_sensitivity(const std::vector<Scan::FPTask_ptr> &tasks, 
                                        const std::string &var);
};

/**
	This class builds the parametric space given a SysVisitor instance.
*/
class SensitivityBuilder {
	SysVisitor *vis;
    int max_bytes;

    template<class IterInt, class IterTask> vector< vector<int> >
    number_of_instances(IterInt pb, IterInt pe, IterTask tb, IterTask te);

    /**
        To compute maximum blocking time of a non-preemptive 
        that (naively) considers pipeline's existence.
     */
    template<class Iter> int compute_max_blocking_time(Iter b, Iter e);

    /** To build the trivial constraints on a node. */
    PPL::C_Polyhedron build_base_constraints(int index);
    /** 
        To build parametric space for preemptive fixed-priority tasks
        on Computational resource node.
    */ 
	ConstraintsSystem build_general_sensitivity( int node);

    /** 
        To build parametric space for non-preemptive fixed-priority tasks
        on Computational resource node.
    */ 
	ConstraintsSystem build_general_sensitivity_np( int node);
	ConstraintsSystem build_general_sensitivity_np_fast( int node);

    /** 
        To build parametric space for tasks
        on Computational resource node.
    */ 
	ConstraintsSystem build_hyperplanes_powerset(int node);

    /** 
        To remove some variables that are not listed in vars_list
        after building a preliminary parametric space on a node.
    */
	void remove_space_dimensions(const int index, const vector<string> &vars, 
                                    PPL::Pointset_Powerset<C_Polyhedron> &poly);
    /** 
        Since remove_space_dimensions has removed some parameters from
        the space, this method updates the variable names.
    */
	vector<string> update_var_names(const int index, const vector<string>&vars);

    /**
        The parametric system space is built incrementaly by merging 
        constraints on each node one by one. After constraints on a
        node has been merged, this method add pipeline constraints
        involving with this node and other already mearged nodes; then
        it removes unnecessary parameters and updates the variable names.
    */
	void merge_pline_constraints( const vector<Scan::FPTask> &pline_tasks, 
                                                        ConstraintsSystem &sys);

public :

	SensitivityBuilder(SysVisitor &sv) { vis = &sv; max_bytes = 0; }

    /** Called to build the parametric space for the whole ststem. */
	ConstraintsSystem build_hyperplanes_powerset();
};

/**
   This function builds a pointset powerset from a set of tasks,
   considering the WCETs are free variables. The method is the same as
   the one in

    Enrico Bini, Giorgio C. Buttazzo, "Schedulability Analysis of
    Periodic Fixed Priority Systems", IEEE Transactions on Computers 53
    (11), pp. 1462-1473, November 2004.

 */
ConstraintsSystem build_hyperplanes_powerset(std::vector<Scan::FPTask> &v);

/**
   This function builds a pointset powerset starting from a set of FP
   tasks, considering WCETs, deadlines and jitters as variables. No
   paper has been published on this method yet.
 */
ConstraintsSystem build_general_sensitivity(std::vector<Scan::FPTask> &v);

PPL::Pointset_Powerset<PPL::C_Polyhedron> 
build_hyperplanes_powerset(std::vector<Scan::FPTask> &v, 
							std::vector<std::string> &vars);

/** this is for EDF */

template<class Iter>
std::vector<int> compute_all_deadlines(Iter a, Iter b)
{
    std::vector<int> dl;
    int h = compute_hyperperiod(a, b, b-a);
    for (Iter p = a; p!=b; ++p) {
        int d = get_next_deadline(*p, 0);
        while (d <= h) {
            dl.push_back(d);
            d = get_next_deadline(*p, d);
        }
    }
    std::sort(dl.begin(), dl.end());
    std::unique(dl.begin(), dl.end());
    return dl;
}

void build_edf_base(const std::vector<Scan::FPTask> &v, PPL::C_Polyhedron &poly, std::vector<std::string> &vars);
void build_edf_constraints(const std::vector<Scan::FPTask> &v, PPL::C_Polyhedron &poly);
int how_many_constraints(const PPL::Constraint_System &cs);

}

#endif

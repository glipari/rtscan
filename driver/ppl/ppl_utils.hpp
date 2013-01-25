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
using namespace std;

namespace PPL = Parma_Polyhedra_Library;
using namespace std;

/** 
    This class is used to read a .scan file and convert it into a set
    of tasks. It applies the "visitor" pattern.
    It can also read a .scan file that describes a distributed system
    with computational resource nodes and pipelines.
*/
class DisSysVisitor : public boost::static_visitor<> {
public:
    /* A copy of all tasks in the system. */
    std::vector<Scan::FPTask> v;
    /*  A set of computational resource nodes in the system. */
    Scan::ComputationalResource *node;
    /*  The number of computational resource nodes in the system. */
    int MAX_;
    /* The pipelines in the system. */
    vector<Scan::Pipeline> pipelines;

    DisSysVisitor(int N):MAX_(N) 
    {
	node = new Scan::ComputationalResource[N];
    }

    void operator()(const Scan::Property &p) {
	if( p.name.compare("Period") != 0 && p.name.compare("E2Edline") != 0)
        	std::cerr << "ERROR: parsing a property at the sys level" 
								<< std::endl;
    }

    void operator()(const Scan::PropertyList &pl) {
        if (pl.type == "sys") 
            for (std::vector< Scan::PropertyList::Element >::const_iterator i = pl.children.begin();
                 i != pl.children.end(); i++) 
                boost::apply_visitor(*this, *i);
	else if (pl.type == "pipeline") {
	    pipelines.push_back(Scan::create_pipeline(pl));
            for (std::vector< Scan::PropertyList::Element >::const_iterator i = pl.children.begin();
                 i != pl.children.end(); i++) 
                boost::apply_visitor(*this, *i);
	}
        else if (pl.type == "task") { 
		Scan::FPTask task = Scan::create_fp_task(pl);
		if( task.get_pipeline_pos() >= 0) {
			// ...
			task.set_pipeline_pos(task.get_pipeline_pos()+1);
			task.set_pipeline_tag(pipelines.back().get_tag());
			pipelines.back().set_pipeline_param(task);
			pipelines.back().register_a_fp_task(task);
		}
		node[task.get_node()-1].register_a_fp_task(task);
		v.push_back(task);
cout<<"task name : "<<task.get_name()<<", scheduling policy : "<<task.get_sched()<<endl;
	}
        else 
            throw std::runtime_error("Found a property list which is not a task!");
			
    }
};

/** 
    This class is used to read a .scan file and convert it into a set
    of tasks. It applies the "visitor" pattern
*/
class SysVisitor : public boost::static_visitor<> {
public:
    std::vector<Scan::FPTask> v;

    SysVisitor() {}

    void operator()(const Scan::Property &p) {
        std::cerr << "ERROR: parsing a property at the sys level" << std::endl;
    }
    void operator()(const Scan::PropertyList &pl) {
        if (pl.type == "sys") 
            for (std::vector< Scan::PropertyList::Element >::const_iterator i = pl.children.begin();
                 i != pl.children.end(); i++) 
                boost::apply_visitor(*this, *i);
        else if (pl.type != "task") 
            throw std::runtime_error("Found a property list which is not a task!");
        else 
            v.push_back(Scan::create_fp_task(pl));
    }
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
	map<double, string> ppl_vars;
    
    ConstraintsSystem(int n);
    ConstraintsSystem(int n, bool universe);

    /**
       This function performs sensitivity analysis over a pointset
       powerset on one single variable, computing maximum and minimum
       values.
    */
    void do_sensitivity(const std::vector<Scan::FPTask> &tasks,
                        const std::string &var);
    void do_sensitivity2(const std::vector<Scan::FPTask> &tasks,
                        const std::string &var);
    void do_sensitivity2(const std::vector<Scan::FPTask> &tasks,
                        const std::string &var1, const std::string &var2, const std::string &fname);
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
ConstraintsSystem build_hyperplanes_powerset2(std::vector<Scan::FPTask> &v);
///void build_hyperplanes_powerset2(vector<Scan::FPTask> &v, ConstraintsSystem &dis);
ConstraintsSystem dis_build_hyperplanes_powerset(DisSysVisitor &vis);
void build_general_sensitivity2(vector<Scan::FPTask> &v, ConstraintsSystem &dis);
void np_build_general_sensitivity(vector<Scan::FPTask> &v, ConstraintsSystem &dis);
void constraints_of_a_pipepline(Scan::Pipeline &pline,
                        DisSysVisitor &vis, ConstraintsSystem &dis);

/**
 * This method builds the same powerset as the above function 
 * for ''non-preemptive scheduling''. For details, please refer 
 * to	http://retis.sssup.it/~youcheng/logs/log0x01.html
**/
ConstraintsSystem np_build_hyperplanes_powerset(std::vector<Scan::FPTask> &v);

/**
   This function builds a pointset powerset starting from a set of FP
   tasks, considering WCETs, deadlines and jitters as variables. No
   paper has been published on this method yet.
 */
ConstraintsSystem build_general_sensitivity(std::vector<Scan::FPTask> &v);

PPL::Pointset_Powerset<PPL::C_Polyhedron> build_hyperplanes_powerset(std::vector<Scan::FPTask> &v,
                                                                     std::vector<std::string> &vars);


// void do_sensitivity(PPL::Pointset_Powerset<PPL::C_Polyhedron> ps, 
//                     const std::vector<std::string> &var_names,
//                     const std::vector<Scan::FPTask> &tasks,
//                     const std::string &var);


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



#endif

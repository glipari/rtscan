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

namespace PPL = Parma_Polyhedra_Library;


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
    
    ConstraintsSystem(int n);

    /**
       This function performs sensitivity analysis over a pointset
       powerset on one single variable, computing maximum and minimum
       values.
    */
    void do_sensitivity(const std::vector<Scan::FPTask> &tasks,
                        const std::string &var);

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

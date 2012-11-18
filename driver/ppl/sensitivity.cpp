#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include "ppl_utils.hpp"
#include <common/string_utils.hpp>

namespace PPL = Parma_Polyhedra_Library;
using namespace std;
using namespace Scan;
using namespace StrUtils;


/** returns the index corresponding to the position in the vector where vname is found, or -1 if 
    vname is not found */
int get_index(const std::vector<string> &var_names, const std::string &vname)
{
    for (unsigned i=0; i<var_names.size(); ++i) 
        if (var_names[i] == vname) return i;
    return -1;
}

/** find the task with a certain name */
int find_task(const std::vector<Scan::FPTask> &tset, const std::string &name)
{
    for (unsigned i=0; i<tset.size(); i++) 
        if (tset[i].get_name() == name) return i;
    return -1;
}

/** 
    Given a task, returns the property with name in var.

    TODO: generalize this function!
 */
double get_value_from_task(const FPTask &task, const std::string &var)
{
    if (var == "wcet") return task.get_wcet();
    else if (var == "period") return task.get_period();
    else if (var == "dline") return task.get_dline();
    else if (var == "jitter") return task.get_jitter();
    else if (var == "offset") return task.get_offset();
    return -1;
}


/**
   given the constraints in ps;
   and given the names of the variables in var_names (in the form "taskname.propname");
   
   1) sets all variables to their values, except for variable var, 
   thus obtaining a constraint set in one single variable

   2) computes max and minimum admissible values for that variable. 
 */
void do_sensitivity(PPL::Pointset_Powerset<PPL::C_Polyhedron> ps, 
                    const std::vector<string> &var_names,
                    const std::vector<Scan::FPTask> &tasks,
                    const std::string &var) 
{
    int k = get_index(var_names, var);   // we do sensitivity on the k variable
    if (k == -1) throw("Variable not found");
    for (int i=0; i<var_names.size(); i++)  {
        if (i == k) continue;
        vector<string> ss = split(var_names[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "ERROR";
        double v = get_value_from_task(tasks[ti], ss[1]);

        Variable xx(i);
        Congruence cg = ((xx %= int(v)) / 0); 
        ps.refine_with_congruence(cg);
    }
    Variable xx(k);
    Linear_Expression le;
    le += (xx);
    Coefficient mn;
    Coefficient md;
    bool is_included;
    ps.maximize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Maximum value for " << var << ": " << mn << "/" <<  md << endl;

    ps.minimize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Minimum value for " << var << ": " << mn << "/" <<  md << endl;
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename> " << endl;
        exit(-1);
    }
    string fname(argv[1]);
    ifstream input(fname.c_str());
    PropertyList sys;
    parse_properties(input, fname, sys);

    PrintPropertyVisitor vis;
    vis(sys);
    
    SysVisitor sv;
    sv(sys);
    
    cout << "Sys parsed!" << endl;

    for (vector<FPTask>::iterator i = sv.v.begin();
         i != sv.v.end(); ++i) 
        cout << *i << endl;

    // tasks have been read, now create the Pointset_Powerset
    vector<string> var_names;
    PPL::Pointset_Powerset<PPL::C_Polyhedron> ps = build_hyperplanes_powerset(sv.v, var_names);

    using namespace PPL::IO_Operators;
    cout << ps << endl;

    for (unsigned char i=0; i<var_names.size(); i++) {
        cout << var_names[i] << endl;
    }

    // now substitute the first n-1 computation times, do analysis on the last one
    for (unsigned i=0; i<sv.v.size() - 1; i++) {
        Variable xx(i);
        Congruence cg = ((xx %= (int)sv.v[i].get_wcet()) / 0); 
        ps.refine_with_congruence(cg);
    }
    cout << ps << endl;
    // find maximum on the wcet:
    // int last = sv.v.size() - 1;
    // Variable xx(last);
    // Linear_Expression le;
    // le += (xx);
    // Coefficient mn;
    // Coefficient md;
    // bool is_included;
    // ps.maximize(le, mn, md, is_included);
    do_sensitivity(ps, var_names, sv.v, string("t3.dline"));
}


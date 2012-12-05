#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include "ppl_utils.hpp"

namespace PPL = Parma_Polyhedra_Library;
using namespace std;
using namespace Scan;



int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename> " << endl;
        exit(-1);
    }
    string fname(argv[1]);
    ifstream input(fname.c_str());
    PropertyList sys;
    try {
        parse_properties(input, fname, sys);
    } catch (std::runtime_error &e) {
        cout << e.what() << endl;
        exit(-1);
    }

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

    try {
        do_sensitivity(ps, var_names, sv.v, string("t3.wcet"));
    }
    catch(char const *msg) {
        cout << "An error occurred while doing sensitivity:" << endl;
        cout << msg << endl;
        exit(-1);
    }
}


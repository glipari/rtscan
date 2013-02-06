#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include <analysis/ppl_utils.hpp>

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

    bool ok = parse_properties(input, fname, sys);
    if (!ok) {
        cout << "Error parsing the file" << endl;
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
    // PPL::Pointset_Powerset<PPL::C_Polyhedron> ps = build_hyperplanes_powerset(sv.v, var_names);

    ConstraintsSystem cs = np_build_hyperplanes_powerset(sv.v);

    using namespace PPL::IO_Operators;
    cout << cs.poly << endl;
    
    for (unsigned char i=0; i<cs.vars.size(); i++) {
        cout << cs.vars[i] << endl;
    }
    
    // // now substitute the first n-1 computation times, do analysis on the last one
    // for (unsigned i=0; i<sv.v.size() - 1; i++) {
    //     Variable xx(i);
    //     Congruence cg = ((xx %= (int)sv.v[i].get_wcet()) / 0); 
    //     ps.refine_with_congruence(cg);
    // }
    // cout << ps << endl;

    try {
        cs.do_sensitivity(sv.v, string("t1.wcet"));
        cs.do_sensitivity(sv.v, string("t2.wcet"));
        cs.do_sensitivity(sv.v, string("t3.wcet"));
    }
    catch(char const *msg) {
        cout << "An error occurred while doing sensitivity:" << endl;
        cout << msg << endl;
        exit(-1);
    }
}


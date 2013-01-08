#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <analysis/task_utility.hpp>
#include <models/task_parser.hpp>
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
    parse_properties(input, fname, sys);

    PrintPropertyVisitor vis;
    vis(sys);
    
    SysVisitor sv;
    sv(sys);

    for (vector<FPTask>::iterator i = sv.v.begin();
         i != sv.v.end(); ++i) 
        cout << *i << endl;
  
    int h = compute_hyperperiod(sv.v.begin(), sv.v.end(), sv.v.size());
    cout << "Hyperperiod: " << h << endl;
    
    // now build the constraint system
    PPL::C_Polyhedron poly(sv.v.size());
    vector<string> vars;

    build_edf_base(sv.v, poly, vars);
    build_edf_constraints(sv.v, poly);
    
    PPL::Constraint_System full_cons_sys = poly.constraints();
    PPL::Constraint_System redu_cons_sys = poly.minimized_constraints();

    int s1 = how_many_constraints(full_cons_sys);    
    int s2 = how_many_constraints(redu_cons_sys);
    
    cout << "Number of constraints (full): " << s1 << endl;
    cout << "Memory (full): " << full_cons_sys.total_memory_in_bytes() << endl;
    cout << "Number of constraints (redu): " << s2 << endl;
    cout << "Memory (redu): " << redu_cons_sys.total_memory_in_bytes() << endl;
    
    

    using namespace PPL::IO_Operators;
    cout << poly << endl;
}

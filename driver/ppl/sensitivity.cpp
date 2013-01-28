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
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <filename> <var>" << endl;
        exit(-1);
    }
    string fname(argv[1]);
    ifstream input(fname.c_str());
    string vname(argv[2]);

    PropertyList sys;

    bool ok = parse_properties(input, fname, sys);
    if (!ok) {
        cout << "Error parsing the file" << endl;
        exit(-1);
   }

    PrintPropertyVisitor vis;
    SysVisitor sv;
    try {
	vis(sys);	
	sv(sys);
    } catch (IllegalValue &err) {
        cout << "An error occurred while parsing the tree:" << endl;
        cout << err.what() << endl;
        exit(-1);
    }
    
    cout << "Sys parsed!" << endl;

    for (vector<FPTask>::iterator i = sv.v.begin();
         i != sv.v.end(); ++i) 
        cout << *i << endl;

    // tasks have been read, now create the Pointset_Powerset
    // PPL::Pointset_Powerset<PPL::C_Polyhedron> ps = build_hyperplanes_powerset(sv.v, var_names);

    ConstraintsSystem cs = build_hyperplanes_powerset(sv.v);

    using namespace PPL::IO_Operators;
    cout << cs.poly << endl;
    
    for (unsigned char i=0; i<cs.vars.size(); i++) {
        cout << cs.vars[i] << endl;
    }
    
    cout << "Doing sensitivity with respect to " << vname << endl;
    try {
        cs.do_sensitivity(sv.v, vname);
    }
    catch(char const *msg) {
        cout << "An error occurred while doing sensitivity:" << endl;
        cout << msg << endl;
        exit(-1);
    }
}


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
    parse_properties(input, fname, sys);

    SysVisitor sv;
    sv(sys);
    
    vector<string> var_names;
    PPL::Pointset_Powerset<PPL::C_Polyhedron> ps =
        build_general_sensitivity(sv.v, var_names);
    
    using namespace PPL::IO_Operators;
    cout << ps << endl;

    for (unsigned char i=0; i<var_names.size(); i++) {
        cout << var_names[i] << endl;
    }

}

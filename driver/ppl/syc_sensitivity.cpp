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

    bool ok = parse_properties(input, fname, sys);
    if (!ok) {
        cout << "Error parsing the file" << endl;
        exit(-1);
    }

    PrintPropertyVisitor vis;
    vis(sys);
    
    DisSysVisitor sv(3);
    sv(sys);
    
    cout << "Sys parsed!" << endl;

    ConstraintsSystem cs = dis_build_hyperplanes_powerset(sv);
    cout<<endl;	
    cout<<"Constraints have been found successfully!\n";
    cout<<endl;	

    using namespace PPL::IO_Operators;
    cout << cs.poly << endl;
    
    for (unsigned char i=0; i<cs.vars.size(); i++) {
		char c = 'A' + i;
        	cout << c << ": " << cs.vars[i] << endl;
    }

}


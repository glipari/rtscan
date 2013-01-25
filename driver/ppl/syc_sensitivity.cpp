#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include <common/string_utils.hpp>
#include "ppl_utils.hpp"

namespace PPL = Parma_Polyhedra_Library;
using namespace std;
using namespace Scan;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <filename> <sensitivity var file> [comparison vars file]" << endl;
        exit(-1);
    }
    string fname(argv[1]);
    string vfilename(argv[2]);

    ifstream input(fname.c_str());
    ifstream varfile(vfilename.c_str());

    PropertyList sys;

    bool ok = parse_properties(input, fname, sys);
    if (!ok) {
        cout << "Error parsing the file" << endl;
        exit(-1);
    }

    PrintPropertyVisitor vis;
    vis(sys);
    
    DisSysVisitor sv(6);
    sv(sys);
    
    cout << "Sys parsed!" << endl;

    ConstraintsSystem cs = dis_build_hyperplanes_powerset(sv);
    cout<<endl;	
    cout<<"Constraints have been found successfully!\n";
    cout<<endl;	

    using namespace PPL::IO_Operators;
//    cout << cs.poly << endl;
//    for( PPL::Pointset_Powerset<PPL::C_Polyhedron>::iterator i = cs.poly.begin(); i != cs.poly.end(); i++) {
//		PPL::Constraint_System csi = i->pointset().constraints();
//		for(PPL::Constraint_System::const_iterator j = csi.begin(); j != csi.end(); j++)
//			cout<<*j<<endl;
//    }
    
    for (unsigned char i=0; i<cs.vars.size(); i++) {
	char c = 'A' + i;
	cout << c << ": " << cs.vars[i] << endl;
    }
    
    cout<<"\nStarting the sensitivity analysis ...\n";

    vector<string> vars;
    while (!varfile.eof()) {
	string line;
	getline(varfile, line);
	line = StrUtils::remove_spaces(line);
	if (line != "") vars.push_back(line);
    }
    try {

	for (unsigned i=0; i < vars.size(); i++) {
	    cs.do_sensitivity2(sv.v, vars[i]);
	    cout << endl;
	}

//	cout<<"(If you see that the lower bound of a deadline is 0, \n";
//	cout<<"it means that the system constraints are not met by current \n";
//	cout<<"configuration values; i.e., the system is not schedulable...)\n\n\n";
    } catch (char const *msg) {cout<<msg<<endl;}

    if( argc != 4) return 0;
    vars.clear();
    string vfilename1(argv[3]);
    string output(argv[3]);
    string fname_(output.append(".out"));
    ifstream varfile1(vfilename1.c_str());
    while (!varfile1.eof()) {
	string line;
	getline(varfile1, line);
	line = StrUtils::remove_spaces(line);
	if (line != "") vars.push_back(line);
    }
    try {
		//cs.do_sensitivity2(sv.v, vars[0], vars[1]);
		cs.do_sensitivity2(sv.v, vars[0], vars[1], fname_);
    } catch (char const *msg) { cout<<msg<<endl; }
    
}


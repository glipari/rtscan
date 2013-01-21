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
//    cout << cs.poly << endl;
    
    for (unsigned char i=0; i<cs.vars.size(); i++) {
		char c = 'A' + i;
        	cout << c << ": " << cs.vars[i] << endl;
    }
    
    cout<<"\nStarting the sensitivity analysis ...\n";
    try {
//    cs.do_sensitivity2(sv.v, string("t2.dline"));
//    cs.do_sensitivity2(sv.v, string("t3.dline"));
//    cs.do_sensitivity2(sv.v, string("t4.dline"));
//    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t51.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t25.dline"));
    cout<<endl;
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t24.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t23.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t22.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t21.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t11.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t11.dline"));
//    cs.do_sensitivity2(sv.v, string("t31.dline"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t51.dline"));
//    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t31.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t19.dline"));
    cout<<endl;
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t2.wcet"));
    cs.do_sensitivity2(sv.v, string("t23.dline"));
    //cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t3.wcet"));
    cs.do_sensitivity2(sv.v, string("t3.dline"));
    cs.do_sensitivity2(sv.v, string("t4.dline"));
    cs.do_sensitivity2(sv.v, string("t5.dline"));
//    cs.do_sensitivity2(sv.v, string("t3.wcet"));
//    cs.do_sensitivity2(sv.v, string("t3.dline"));
    cs.do_sensitivity2(sv.v, string("t25.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t13.dline"));
    cout<<endl;
    cs.do_sensitivity2(sv.v, string("t3.dline"));
    cout<<endl;
    return 0;
//    cs.do_sensitivity2(sv.v, string("t5.dline"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t5.wcet"));
    cs.do_sensitivity2(sv.v, string("t25.dline"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t21.wcet"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t22.wcet"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t23.wcet"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t24.wcet"));
    cout<<endl;
//    cs.do_sensitivity2(sv.v, string("t25.wcet"));
    cout<<endl;
    cout<<"(If you see that the lower bound of a deadline is 0, \n";
    cout<<"it means that the system constraints are not met by current \n";
    cout<<"configuration values; i.e., the system is not schedulable...)\n\n\n";
    } catch (char const *msg) {cout<<msg<<endl;}
}


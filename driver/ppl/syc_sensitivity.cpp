#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include <common/string_utils.hpp>
#include <analysis/ppl_utils.hpp>

namespace PPL = Parma_Polyhedra_Library;
using namespace std;
using namespace Scan;

int main(int argc, char *argv[])
{
    if (argc < 4) {
        cout << "Usage:   syc_sensitivity " << "   <filename> 	    	  : the .scan input file \n";
	cout << "                         " << "   <vars-list file> 	  : this file specifies free variables. Defaultly, jitter and\n";
	cout << "			  " << "				dline of a pipeline task is the free parameter and there\n";
	cout << "			  " << "				is no need to put them in this file\n"; 
  	cout << "			  " << "   <do-sensitivity vars>  : RTSCAN will do analysis for variale in this file\n"; 
	cout << "			  " << "   [comparison vars file] : RTSCAN will build the parametric space for the pair of\n";
	cout << "			  " << "				variables here\n";
        exit(-1);
    }
    string fname(argv[1]);
    string vars_list_file(argv[2]);
    string vfilename(argv[3]);

    ifstream input(fname.c_str());
    ifstream varfile(vfilename.c_str());
    ifstream vars_list_stream(vars_list_file.c_str());

    PropertyList sys;

    bool ok = parse_properties(input, fname, sys);
    if (!ok) {
        cout << "Error parsing the file" << endl;
        exit(-1);
    }

    PrintPropertyVisitor vis;
    vis(sys);
    
    DisSysVisitor sv(11);
    sv(sys);
    
    cout << "Sys parsed!" << endl;

    vector<string> vars_list;
    while (!vars_list_stream.eof()) {
	string line;
	getline(vars_list_stream, line);
	line = StrUtils::remove_spaces(line);
	if (line != "") vars_list.push_back(line);
    }

    ConstraintsSystem cs = dis_build_hyperplanes_powerset(sv, vars_list);
    cout<<endl;	
    cout<<"Constraints have been found successfully!\n";
    cout<<endl;	

    using namespace PPL::IO_Operators;
    cout<<cs.poly<<endl;
    cs.print_points(vars_list_file);
//    ofstream res;
//    string out = vars_list_file.append(".out");
//    res.open(out.c_str());
//
    cout<<"The final system :\n"<<endl;
    PPL::Pointset_Powerset<C_Polyhedron> copied(cs.poly);
    cout<<copied<<endl;
    for( PPL::Pointset_Powerset<PPL::C_Polyhedron>::iterator i = copied.begin(); i != copied.end(); i++) {
                PPL::C_Polyhedron cp = i->pointset();
                PPL::Generator_System gs = cp.generators();
		cout<<gs<<endl;
    }
//		for( PPL::Generator_System::const_iterator it = gs.begin(); it != gs.end(); it++) {
//			if( !it->is_point()) throw("Not a point here");
//			for( PPL::dimension_type ii = it->space_dimension(); ii -- > 0;)
//				cout<<it->coefficient(Variable(ii))<<". ";
//			cout<<endl;
//		}
//                //cout<<gs<<endl;
////		PPL::Constraint_System csi = i->pointset().constraints();
////                for(PPL::Constraint_System::const_iterator j = csi.begin(); j != csi.end(); j++) {
//                        res<<*j<<endl;
//
//                }
//                res<<endl;
//
//    }
//    cout<<endl;
    
    for (unsigned char i=0; i<cs.vars.size(); i++) {
	char c = 'A' + i;
	cout << c << ": " << cs.vars[i] << endl;
    }
    cout<<"Total memory used in M bytes (resulted pointset_powerset): "<< (cs.poly.total_memory_in_bytes()/1024)/1024 << endl;
    
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
	    //cs.do_sensitivity2(sv.v, vars[i]);
	    cs.do_sensitivity3(sv.v, vars[i]);
	    cout << endl;
	}

    } catch (char const *msg) {cout<<msg<<endl;}

    if( argc != 5) return 0;
    vars.clear();
    string vfilename1(argv[4]);
    string output(argv[4]);
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


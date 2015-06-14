#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <boost/program_options.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include <common/string_utils.hpp>
#include <analysis/ppl_utils.hpp>

namespace PPL = Parma_Polyhedra_Library;
namespace po = boost::program_options;
using namespace std;
using namespace Scan;


int main(int argc, char *argv[])
{

    po::options_description options(argv[0]);
    options.add_options()
        ("help,h", "Use -h or --help to list all arguments")
        ("file,f", po::value<string>(), "The input file")
        ("num,n", po::value<int>(), "The number of computational resource nodes. By default 10 will be set.")
        ("vars,v", po::value<string>(), "Provide the file containing variable list. By default the wcet, dline and jitter of all tasks will be free variables.")
        ("analysis,a", po::value<string>(), "Provide the file that contains variables for sensitivity analysis")
        ("print,p", po::value<string>(), "To print the resulting Pointset_Powerset into files");

    po::variables_map vm;
    po::store( po::parse_command_line(argc, argv, options), vm);
    po::notify(vm);

    if( vm.count("help")) {
        cout << options << endl;
    }

    if(!vm.count("file")) {
        cout << options << endl;
        exit(1);
    }

    string fname (vm["file"].as<string>());
    ifstream input(fname.c_str());
    PropertyList sys;

    /** To parse the input file. */
    bool ok = parse_properties(input, fname, sys);
    if (!ok) {
        cout << "Error parsing the file" << endl;
        exit(-1);
    }
    
    /** To print the input file. */
    PrintPropertyVisitor vis;
    vis(sys);

    int num=10;
    if( vm.count("num")) num = vm["num"].as<int>();
    /** 
        To collect system information : tasks and their (free and fixed) 
	    parameters. 
     */
    SysVisitor sv(num);
    sv(sys);

    if( vm.count("vars")) 
        sv(vm["vars"].as<string>());
    else sv("");

    cout << "Sys parsed!" << endl;

    /** To build the parametric space. */
	SensitivityBuilder sb(sv);
    ConstraintsSystem cs = sb.build_hyperplanes_powerset();

    /** This part can be encapsulated inside a member function. */
    using namespace PPL::IO_Operators;
    cout<<cs.poly<<endl;
    for (unsigned char i=0; i<cs.vars.size(); i++) {
        char c = 'A' + i;
        cout << c << ": " << cs.vars[i] << endl;
    }
    cout << "Total memory used in  bytes (resulted pointset_powerset): ";
    cout << cs.poly.total_memory_in_bytes() << endl;

    /** To do sensitivity analysis. */
    if( vm.count("analysis")) {
        cout<<"\nStarting the sensitivity analysis ...\n";
        string sens_file(vm["analysis"].as<string>());
	    ifstream sens_stream(sens_file.c_str());
	    vector<string> sens_vars;
	    while (!sens_stream.eof()) {
		    string line;
		    getline(sens_stream, line);
		    line = StrUtils::remove_spaces(line);
		    if (line != "") sens_vars.push_back(line);
	    }
	    try {
		    for (unsigned i=0; i < sens_vars.size(); i++)
		        cs.do_sensitivity(sv.all_tasks, sens_vars[i]);
            
	    } catch (char const *msg) { cout<<msg<<endl; }
    }

    if( vm.count("print"))
        cs.print_points(vm["print"].as<string>());

    return 0; 
}


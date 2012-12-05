#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>
#include <analysis/fp_response_time.hpp>
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
    } catch(const std::runtime_error &e) {
        cout << e.what() << endl;
        exit(-1);
    }

    SysVisitor sv;
    sv(sys);
    
    ConstraintsSystem s1 = build_general_sensitivity(sv.v);
    
    //using namespace PPL::IO_Operators;
    //cout << s1.poly << endl;

    for (unsigned char i=0; i<s1.vars.size(); i++) {
        char c = 'A' + i;
        cout << c << ": " << s1.vars[i] << endl;
    }
    // substitute everything but t3.dline

    do_sensitivity(s1.poly, s1.vars, sv.v, "t3.dline");  
    
    double rt = resp_time(sv.v.begin(), sv.v.end(), 1000);
    cout << "Response time of t3: " << rt << endl;
}

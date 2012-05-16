#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include <common/sequence.hpp>
#include <analysis/fmlp.hpp>
#include <analysis/mbwi_interference.hpp>

#include "mbwi_sys.hpp"

using namespace std;
using namespace Scan;

bool isEmpty(const string &str) 
{
    size_t endpos = str.find_first_not_of(" \t\n");
    if (endpos == string::npos) return true;
    return false;
}

vector<TaskRes> read_input_file(ifstream &file, sys_params &sys)
{
    string line;

    getline(file, line);
    if (file.eof()) return sys.tset;
    while (!isEmpty(line)) {
        vector<string> toks; 
        string_split(line, " ,\t", back_inserter(toks));
        if (toks.size() < 4) throw "Not enough elements!";

        vector<double> val(toks.size());
        for (unsigned i=0; i<toks.size(); i++) 
            val[i] = atof(toks.at(i).c_str());

        TaskRes t(val.at(2), val.at(3), val.at(3));
        sys.tset.push_back(t);
        cout << t << endl;
        getline(file, line);
    } 
    return sys.tset;
}

int main(int argc, char *argv[])
{
    int c;
    sys_params sys;

    sys.parse_args(argc, argv);

    ifstream is(sys.input_filename.c_str());
    while (1) {
        vector<TaskRes> tset;
        try {
            sys.tset = read_input_file(is, sys);
        } catch (const char *error) {
            cout << error << endl;
        }
        if (sys.tset.size() == 0) break;
        
        sys.check();

        sys.generate_resources();
        
        sys.generate_crit_sections();
        
        /**
           DONE!

           Now test schedulability
           The compute interference with MBWI, 
           add to computation time, 
           test schedulability
           
           Do the same with FMLP.
        */
        /**
           Finally, I have to output. 
           MBWI_INTERF   interf
           FMLP_BLOCKING blocking 
           
           SCHED_NORES  Yes or No
           SCHED_MBWI   Yes or No
           SCHED_FMLP   Yes or No
           TESKSAT
        */
    }

    return 0;
}

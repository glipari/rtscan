#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include <common/sequence.hpp>
#include <common/generators.hpp>
#include <analysis/fmlp.hpp>
#include <analysis/omlp.hpp>
#include <analysis/mbwi_interference.hpp>
#include <analysis/medf_rta.hpp>

#include "mbwi_sys.hpp"

using namespace std;
using namespace Scan;

bool isEmpty(const string &str) 
{
    size_t endpos = str.find_first_not_of(" \t\n");
    if (endpos == string::npos) return true;
    return false;
}

void read_input_file(ifstream &file, sys_params &sys)
{
    string line;
    
    vector<TaskRes> tset;

    getline(file, line);
    if (file.eof()) {
        sys.tset.clear();
        return;
    }
    while (!isEmpty(line)) {
        vector<string> toks; 
        string_split(line, " ,\t", back_inserter(toks));
        if (toks.size() < 4) throw "Not enough elements!";
        
        vector<double> val(toks.size());
        for (unsigned i=0; i<toks.size(); i++) 
            val[i] = atof(toks.at(i).c_str());
        
        TaskRes t(val.at(2), val.at(3), val.at(3));
        tset.push_back(t);
        cout << "Task: " << t << endl;
        getline(file, line);
    } 
    sys.tset = tset;
}

int main(int argc, char *argv[])
{
    sys_params sys;

    int nsets = 0;
    int nsched = 0;
    int nsched_mbwi = 0;
    int nsched_fmlp = 0;
    int nsched_omlp = 0;

    double util = 0;

    sys.parse_args(argc, argv);
    sys.print_params();
    sys.check();

    ifstream is(sys.input_filename.c_str());
    GeneratorSingleton::init(sys.rndseed);

    while (!is.eof()) {
        try {
            read_input_file(is, sys);
        } catch (const char *error) {
            cout << error << endl;
            exit(-1);
        }
        if (sys.tset.size() == 0) {
            cout << "No more task sets to process, exiting!" << endl;
            break;
        }

        nsets++;
        cout << "Number of tasks: " << sys.tset.size() << endl;
        
        util = 0;
        for (auto t : sys.tset) util+=t.get_wcet()/t.get_period();
        cout << "Task utilization: " << util << endl;

        cout << "Task Set has been read, now checking schedulability" << endl;
        if (gedf_iterative(sys.nproc, sys.tset.begin(), sys.tset.end())) {
            nsched++;
            cout << "task set schedulable, now generating resources" << endl;
            sys.generate_resources();
            cout << "Generate " << sys.rgroups.size() << " resource groups" << endl;
            for (unsigned i=0; i<sys.rgroups.size(); ++i) 
                cout << "    group " << i << " has " << sys.rgroups[i].size() << " resources" << endl;

            cout << "generating critical sections" << endl;            
            sys.generate_crit_sections();
            cout << "now testing with mbwi" << endl;
            vector<TaskRes> tset_mbwi;
            bool sched_mbwi = false;
            try {
                for (auto t : sys.tset) {
                    double Interf = mbwi_compute_interference(t, sys.tset);
                    cout << " MBWI interference for " << t << " : " << Interf << endl;
                    t.set_wcet(t.get_wcet() + Interf);
                    tset_mbwi.push_back(t);
                }
                for (auto t : sys.tset) cout << t << endl;
                for (auto t : tset_mbwi) cout << t << endl;
                
                sched_mbwi = gedf_iterative(sys.nproc, 
                                            tset_mbwi.begin(), 
                                            tset_mbwi.end());
            } catch (IllegalValue &err) {
                cout << "Not schedulable with MBWI" << endl;
                sched_mbwi = false;
            }

            vector<TaskRes> tset_fmlp;
            bool sched_fmlp = false;
            try { 
                FMLPAnalysis fmlp(sys.nproc, 
                                  sys.tset.begin(), sys.tset.end(), 
                                  sys.all_res.begin(), sys.all_res.end());
                for (auto t : sys.tset) { 
                    double blocking = fmlp.blocking_time(t);
                    cout << " FMLP blocking for " << t << " : " << blocking << endl;

                    t.set_wcet(t.get_wcet() + blocking);
                    tset_fmlp.push_back(t);
                }
                sched_fmlp = gedf_iterative(sys.nproc, tset_fmlp.begin(), tset_fmlp.end());
            } catch(IllegalValue &err) {
                cout << "Not schedulable with FMLP" << endl;
                sched_fmlp = false;
            }

            vector<TaskRes> tset_omlp;
            bool sched_omlp = false;
            try { 
                OMLPAnalysis omlp(sys.nproc, 
                                  sys.tset.begin(), sys.tset.end(), 
                                  sys.all_res.begin(), sys.all_res.end());
                for (auto t : sys.tset) { 
                    double blocking = omlp.blocking_time_global(t);
                    cout << " OMLP blocking for " << t << " : " << blocking << endl;

                    t.set_wcet(t.get_wcet() + blocking);
                    tset_omlp.push_back(t);
                }
                sched_omlp = gedf_iterative(sys.nproc, tset_omlp.begin(), tset_omlp.end());
            } catch(IllegalValue &err) {
                cout << "Not schedulable with OMLP" << endl;
                sched_omlp = false;
            }

            cout << "Schedulable by mbwi: " << sched_mbwi << endl;
            cout << "Schedulable by fmlp: " << sched_fmlp << endl;
            cout << "Schedulable by omlp: " << sched_omlp << endl;            

            if (sched_mbwi) nsched_mbwi++;
            if (sched_fmlp) nsched_fmlp++;
            if (sched_omlp) nsched_omlp++;
        }
        else {
            cout << "Task set is not scheduable" << endl;
        }
    }

    ofstream os(sys.output_filename.c_str(), ios::app);
    os << util << ", " 
       << sys.nproc << ", " 
       << sys.tset.size() << ", " 
       << nsets << ", " 
       << nsched << ", " 
       << nsched_mbwi << ", " 
       << nsched_fmlp << ", " 
       << nsched_omlp << endl;

    os.close();

    return 0;
}

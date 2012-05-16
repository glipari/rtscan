#include <iostream>
#include <cstdlib>
#include <cstring>
#include <random>

#include "mbwi_sys.hpp"

#include <common/generators.hpp>

using namespace std;
using namespace Scan;

void sys_params::parse_args(int argc, char *argv[]) {
    int c;
    while((c = getopt(argc,argv,"f:m:h:o:s:l:1:2:3:z:g")) != -1) {
        switch (c) {
        case 'h' : 
            print_help();
            break;
        case 'm': 
            nproc = atoi(optarg);
            break;
        case 'f': 
            input_filename = string(optarg);
            break;
        case 'o': 
            output_filename = string(optarg);
            break;
        case 's': 
            nshort = atoi(optarg);
            break;
        case 'l': 
            nlong = atoi(optarg);
            break;
        case '1': 
            prob[0] = atof(optarg);
            break;
        case '2': 
            prob[1] = atof(optarg);
            break;
        case '3': 
            prob[2] = atof(optarg);
            break;
        case 'z': 
            nesting_prob = atof(optarg);
            break;
        case 'g': 
            ngroups = atoi(optarg);
            break;
        case 'd': 
            min_dur = atof(optarg);
            break;
        case 'D': 
            max_dur = atof(optarg);
            break;
        case 't': 
            threshold = atof(optarg);
            break;
        }
    }
}

void sys_params::print_help()
{
    cout << "-h  : this help" << endl;
    cout << "Help for mbwi_simul" << endl;
    cout << "-f  : file containing the task set (default = tset.txt) (see taskgen.py for more info)" << endl;
    cout << "-o  : output file (default = tres.txt)" << endl;
    cout << "-m  : number of processors (default = 2) " << endl;
    cout << "-s  : number of short resources (default = 2) " << endl;
    cout << "-l  : number of long resources (default = 0) " << endl;
    cout << "-1  : probability of one critical section (default = 1) " << endl;
    cout << "-2  : probability of two critical sections (default = .5) " << endl;
    cout << "-3  : probability of three critical sections (default = .1) " << endl;
    cout << "-z  : probability of nesting (default = .5) " << endl;
    cout << "-g  : number of resource groups (default = 1) " << endl; 
    cout << "-d  : min duration of cs (default = 5 (usec)) " << endl; 
    cout << "-D  : max duration of cs (default = 500 (usec)) " << endl; 
    cout << "-t  : threshold short/long (default = 50 (usec)) " << endl; 
}

void sys_params::check()
{
}

void sys_params::generate_resources()
{    
    if (ngroups > nshort + nlong) THROW_EXC(TooManyGroups, "More groups than resources");
    rgroups.clear();
    vector<pair<int,int>> vs;
    vector<pair<int,int>> vl;
    for (int i =0; i<ngroups; ++i) {
        vs.push_back(pair<int,int>(0, nshort));
        vl.push_back(pair<int,int>(0, nlong));
    }
    vector<int> ns = random_bins(vs, nshort);
    vector<int> nl = random_bins(vl, nlong);

    /**
       Then, generate nlong resources and nshort resources.
       Then assign resources to groups.
    */
    for (int i=0; i<ngroups; ++i) {
        vector<Resource> rg;
        for (int j=0; j<ns[i]; ++j) 
            rg.push_back(Resource(true));
        rgroups.push_back(rg);
    }

    for (int i=0; i<ngroups; ++i) {
        for (int j=0; j<nl[i]; ++j) 
            rgroups[i].push_back(Resource(false));
    }
}

void sys_params::generate_crit_sections()
{
    /**
       Assign tasks to groups. A group can only access
       critical sections from its assigned group.
    */
    // divides all tasks in groups
    vector<pair<int,int>> vt;
    for (int i =0; i<ngroups; ++i) {
        vt.push_back(pair<int,int>(1, tset.size()));
    }
    // number of tasks per each group
    vector<int> tg = random_bins(vt, tset.size()); 
    
    // associates tasks to group
    vector<int> task_group;
    int g = 0;
    for (unsigned i=0; i<tset.size();i++) {
        if (tg[g] == 0) g++;
        else tg[g]--;
        task_group[i] = g;
    }
    
    /**
       Now, generate critical sections according to the
       probability. 
       
       For each task, generate probability. If a CS has to be
       created, select a random resource from the same group.
       
       The duration has lognormal distribution, between min_dur
       and threshold if short, and between threshold and max_dur
       if long.
       
       Each critical section can be nested, according to the
       nesting_prob probability. 
       
       The id of the nested resource must be greater than the id
       of the containing one (this ensures that no long resource
       can get inside a short one). Therefore, even if the
       probability has hit, then it may be the case than there is
       no nested critical section.
              
       Then generate probability for the second CS, and so on.
    */
    for (unsigned i = 0; i<tset.size(); ++i) { 
        int gindex = task_group[i];
        int n_res_in_group = rgroups[gindex].size();
        if (n_res_in_group > 0) {
            std::uniform_real_distribution<double> p_cs_dist(0,1);
            std::uniform_int_distribution<int> p_n_dist(0, n_res_in_group-1);
            double p_cs = p_cs_dist(RNDGEN());
            int ncs = 0;
            if (p_cs < prob[2]) ncs = 3;
            else if (p_cs < prob[1]) ncs = 2;
            else if (p_cs < prob[0]) ncs = 1;
            else ncs = 0;
            for (unsigned k = 0; k<ncs; k++) { 
                unsigned rn = p_n_dist(RNDGEN());
                bool is_short = rgroups[gindex][rn].is_short();
                double dur = 0;
                if (is_short) {
                    std::uniform_real_distribution<double> p_dur(min_dur,threshold);
                    dur = p_dur(RNDGEN());
                }
                else {
                    std::uniform_real_distribution<double> p_dur(threshold, max_dur);            
                    dur = p_dur(RNDGEN());
                }
                CriticalSection crit_sect(rgroups[gindex][rn].get_id(), dur);
                double p_nesting = p_cs_dist(RNDGEN());
                // if (p_nesting < nesting_prob) 
                //     create_crit_sect();
            }
        }
        
    }
        
}

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <random>
#include <algorithm>
#include <ctype.h>
#include <cstdio>
#include <unistd.h>

#include "mbwi_sys.hpp"

#include <common/generators.hpp>
#include <common/sequence.hpp>

using namespace std;
using namespace Scan;

void sys_params::parse_args(int argc, char *argv[]) {
    int c;
    while((c = getopt(argc,argv,"hf:m:o:s:l:1:2:3:z:g:r:x:y:t:p")) != -1) {
        switch (c) {
        case 'h' : 
            print_help();
            exit(0);
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
        case 'r': 
            rndseed = atoi(optarg);
            break;
        case 'x': 
            max_tasks_per_group = atoi(optarg);
            break;
        case 'y': 
            min_tasks_per_group = atoi(optarg);
            break;
        case 'p': 
            omlptest = true;
            break;
        }
    }
}

void sys_params::print_params()
{
    cout << "Input file (-f): " << input_filename << endl;
    cout << "Output file (-0): " << output_filename << endl;

    cout << "random seed (-r): " << rndseed << endl;
    cout << "Number of processors (-m): " << nproc << endl;
    cout << "Number of short res (-s): " << nshort << endl;
    cout << "Number of long res (-l): " << nlong << endl;
    cout << "probability of one critical section (-1): " << prob[0] << endl;
    cout << "probability of two critical sections (-2): " << prob[1] << endl;
    cout << "probability of three critical sections (-3): " << prob[2] << endl;
    cout << "probability of nesting (-z): " << nesting_prob << endl;
    cout << "number of res groups (-g): " << ngroups << endl;
    cout << "min duration of cs (-d): " << min_dur << endl;
    cout << "max duration of cs (-D): " << max_dur << endl;
    cout << "threshold short/long (-t): " << threshold << endl;
    cout << "max tasks per group (-x): " << max_tasks_per_group << endl;
    cout << "min tasks per group (-y): " << min_tasks_per_group << endl;
    cout << "enable omlp test (-p) " << omlptest << endl;
}

void sys_params::print_help()
{
    cout << "Help for mbwi_simul" << endl;
    cout << "    -h  : this help" << endl;
    cout << "    -f  : file containing the task set (default = tset.txt) (see taskgen.py for more info)" << endl;
    cout << "    -o  : output file (default = tres.txt)" << endl;
    cout << "    -m  : number of processors (default = 2) " << endl;
    cout << "    -s  : number of short resources (default = 2) " << endl;
    cout << "    -l  : number of long resources (default = 0) " << endl;
    cout << "    -1  : probability of one critical section (default = 1) " << endl;
    cout << "    -2  : probability of two critical sections (default = .5) " << endl;
    cout << "    -3  : probability of three critical sections (default = .1) " << endl;
    cout << "    -z  : probability of nesting (default = .5) " << endl;
    cout << "    -g  : number of resource groups (default = 1) " << endl; 
    cout << "    -d  : min duration of cs (default = 5 (usec)) " << endl; 
    cout << "    -D  : max duration of cs (default = 500 (usec)) " << endl; 
    cout << "    -t  : threshold short/long (default = 50 (usec)) " << endl; 
    cout << "    -r  : random seed (default = 12345) " << endl; 
    cout << "    -x  : max tasks per group (default = 6) " << endl; 
    cout << "    -y  : min tasks per group (default = 2) " << endl; 
    cout << "    -p  : enable omlp test (default = false), requires nesting prob = 0" << endl; 
}

void sys_params::check()
{

    if (omlptest && nesting_prob > 0) { 
        cout << "omlp test requires nesting probability = 0" << endl;
        exit(-1);
    }
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
    for (int i=0; i<ngroups; ++i) 
        copy(rgroups[i].begin(), rgroups[i].end(), back_inserter(all_res));    
}

void sys_params::generate_crit_sections()
{
    /**
       Assign tasks to groups. A group can only access
       critical sections from its assigned group.
    */
    cout << "Assigning tasks to groups" << endl;
    // divides all tasks in groups
    vector<pair<int,int>> vt;
    for (int i =0; i<ngroups; ++i) 
        vt.push_back(pair<int,int>(min_tasks_per_group, max_tasks_per_group));
    
    // number of tasks per each group
    vector<int> tg = random_bins(vt, tset.size()); 
    
    int tot = 0;
    for_each(tg.begin(), tg.end(), [&tot](int x) { tot+=x;});
    cout << "generated " << tg.size() << " random bins" << endl;
    cout << "for a total of " << tot << " tasks" << endl;

    // associates tasks to group
    vector<int> task_group;
    int g = 0;
    for (unsigned i=0; i<tset.size();i++) {
        if (tg[g] == 0) g++;
        else tg[g]--;
        task_group.push_back(g);
    }
    
    cout << "Tasks associated to groups" << endl;

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
        cout << "For task " << i << endl;
        int gindex = task_group[i];
        cout << "    Group index: " << gindex << endl;
        CSSet csset;
        create_crit_sections(csset, -1, false, gindex);
        cout << "    " << csset.size() << " critical sections created, now adding to the task" << endl;
        for (auto cs: csset) 
            tset[i].addCS(cs);
    }

}

int level = 0;

// res_min, min index of the resource. Used to guarantee the access order
// s: if the containing critical section is short (true) or long (false)
// gindex, the group index
void sys_params::create_crit_sections(CSSet &csset, int res_min, bool s, int gindex)
{
    csset.clear();
    vector<Resource> temp;
    select(rgroups[gindex].begin(), rgroups[gindex].end(), back_inserter(temp),  
           [&res_min](Resource i) { return i.get_id() > res_min;});
    vector<Resource> goodres;
    select(temp.begin(), temp.end(), back_inserter(goodres),
           [&s](Resource i) { return !( !i.is_short() && s);});
    if (goodres.size() == 0) return;
    
    for (int i=0;i<level+1;i++) cout << "    ";
    cout << "selected " << goodres.size() << " resources " << endl;

    std::uniform_int_distribution<int> p_n_dist(0, goodres.size()-1);
    std::uniform_real_distribution<double> p_cs_dist(0,1);
    
    double p_cs = p_cs_dist(RNDGEN());
    int ncs = 0;
    if (p_cs < prob[2]) ncs = 3;
    else if (p_cs < prob[1]) ncs = 2;
    else if (p_cs < prob[0]) ncs = 1;
    else ncs = 0;
    
    for (int i=0;i<level+1;i++) cout << "    ";
    cout << "generating " << ncs << " critical sections" << endl;

    for (unsigned k = 0; k<ncs; k++) { 
        unsigned rn = p_n_dist(RNDGEN());
        bool is_short = goodres[rn].is_short();
        double dur = 0;
        if (is_short) {
            std::uniform_real_distribution<double> p_dur(min_dur,threshold);
            dur = p_dur(RNDGEN());
        }
        else {
            std::uniform_real_distribution<double> p_dur(threshold, max_dur);        
            dur = p_dur(RNDGEN());
        }
        
        for (int i=0;i<level+1;i++) cout << "    ";
        cout << "created a critical " << (is_short ? "short" : "long") << " section of " << dur << " on res " << goodres[rn].get_id() << endl;
        CriticalSection crit_sect(goodres[rn].get_id(), dur);
        double p_nesting = p_cs_dist(RNDGEN());
        if (p_nesting < nesting_prob) {
            CSSet nested;
            level++;
            create_crit_sections(nested, goodres[rn].get_id(), is_short, gindex);
            level--;
            for (auto cs : nested) 
                crit_sect.addNestedCS(cs);
        }
        csset.push_back(crit_sect);
    }
}    

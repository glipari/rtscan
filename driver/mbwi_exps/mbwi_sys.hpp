#ifndef __MBWI_SYS_HPP__
#define __MBWI_SYS_HPP__

#include <string>
#include <vector>
#include <common/exceptions.hpp>

#include <models/taskres.hpp>

struct sys_params {
    std::string input_filename; // = "tset.txt";
    std::string output_filename;// = "tres.txt";

    int nproc; // = 2;                 // number of processors
    int nshort; // = 2;                // number of short resources
    int nlong; // = 0;                 // number of long resources

    double prob[3]; // = {1, .5, .1};
    double nesting_prob; // = .5;
    int ngroups; // = 1;               // resources are divided into ngroups. 
                                   // a task can only use resources from one group
                                   // this is to test the advantages of isolation

    /**
       Computation time is assumed to be in milliseconds
       Therefore, the following are also in milliseconds
    */
    double min_dur; // = 0.005;        
    double max_dur; // = 0.500;
    double threshold; // = 0.050;
    int rndseed;
    int max_tasks_per_group;
    int min_tasks_per_group;

    std::vector<Scan::TaskRes> tset;
    std::vector<std::vector<Scan::Resource>> rgroups;
    std::vector<Scan::Resource> all_res;
    
    sys_params() : input_filename("tset.txt"), 
                   output_filename("tres.txt"),
                   nproc(2),
                   nshort(2),
                   nlong(0),
                   prob({1, .5, .1}),
                   nesting_prob(.5),
                   ngroups(1),
                   min_dur(.005),
                   max_dur(.5),
                   threshold(.05),
                   rndseed(12345),
                   max_tasks_per_group(6),
                   min_tasks_per_group(2),
                   tset(),
                   rgroups()
        {}

    void print_help();
    void print_params();
    void parse_args(int argc, char *argv[]);
    void check();
    void generate_resources();
    void generate_crit_sections();
    void create_crit_sections(Scan::CSSet &csset, int res_min, bool upper, int gindex);
};

DECL_EXC(TooManyGroups);


#endif

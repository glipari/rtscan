#ifndef SYM_PARAM_HPP_INCLUDED
#define SYM_PARAM_HPP_INCLUDED
#include <string>
#include <vector>
#include <common/exceptions.hpp>
#include <utility>
#include <map>
#include <models/transaction.hpp>
#include <models/Processor.hpp>

struct Sym_Params
{
    string input_filename; // = "tset.txt";
    string output_filename;// = "tres.txt";
    string input_processor_filename;

    int nproc; // = 2;                 // number of processors



    /**
       Computation time is assumed to be in milliseconds
       Therefore, the following are also in milliseconds
    */
    double min_dur; // = 0.005;
    double max_dur; // = 0.500;
    double threshold; // = 0.050;
    int rndseed;
    vector< pair<double,double> >period_deadline;
    vector<Processor> processors;



    vector<Scan::TaskSet> pipes_data;


    Sym_Params() : input_filename("tset.txt"),
        output_filename("tres.txt"),
        input_processor_filename("proc.txt"),
        nproc(2),

        min_dur(.005),
        max_dur(.5),
        threshold(.05),

        pipes_data(),
        period_deadline(),
        processors()

    {
    }

    void print_help();
    void print_params();
    void parse_args(int argc, char *argv[]);

};




#endif // SYM_PARAM_HPP_INCLUDED

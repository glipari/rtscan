#ifndef PIPE_GENERATOR_HPP_INCLUDED
#define PIPE_GENERATOR_HPP_INCLUDED
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <cmath>
#include <random>


//using std::endl;



//using std::ofstream;
namespace Scan
{

typedef std::mt19937 rndgen_t;

class Pipe_Generator
{
    static rndgen_t *rng;
    std::string filename;
    int number_of_pipe;
    int num_proc;
    std::ofstream outfile;
     int give_num_stage(int num_proc);
    double compute_period();

    void write_on_file( std::string line);
    double compute_deadline(double period);
     std::vector<double> compute_wcet(int num_stag,int period);


    public:
    Pipe_Generator();
    Pipe_Generator(std::string f, int npipe, int num_pr);


    void generator();


};
}
#endif // PIPE_GENERATOR_HPP_INCLUDED

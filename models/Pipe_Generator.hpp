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
#include <common/UsefulF.hpp>


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
    int seed;
    double x;
    int num_task;
    std::ofstream outfile;
    std::ofstream outf;
     int give_num_stage();
    double compute_period();
    void write_on_file( std::string line);
    double compute_deadline(double period);
    std::vector<double> compute_wcet(int num_stag,int period);

    public:
    Pipe_Generator();
    Pipe_Generator(int seed);
    Pipe_Generator(std::string f, int npipe,int seed,double x_u, int n_task);
    void generator();


};
}
#endif // PIPE_GENERATOR_HPP_INCLUDED

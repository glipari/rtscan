#include <iostream>
#include <cstdlib>
#include <cstring>
#include <random>
#include <algorithm>

#include "models/Sym_Param.hpp"

#include <common/sequence.hpp>

using namespace std;
using namespace Scan;

void Sym_Params::parse_args(int argc, char *argv[]) {
    int c;
    while((c = getopt(argc,argv,"h:f:o:t:d:D:p:r")) != -1) {
        switch (c) {
        case 'h' :
            print_help();
            exit(0);
            break;

        case 'f':
        {
            cout<<"file name :"<<optarg<<endl;;
             input_filename = string(optarg);
            break;
        }

        case 'o':
            output_filename = string(optarg);
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
        case 'p':
        input_processor_filename = string(optarg);

        }
    }
}

void Sym_Params::print_params()
{
    cout << "Input file (-f): " << input_filename << endl;
    cout << "Input file (-p): " << input_processor_filename << endl;
    cout << "Output file (-0): " << output_filename << endl;

    cout << "random seed (-r): " << rndseed << endl;

    cout << "min duration of cs (-d): " << min_dur << endl;
    cout << "max duration of cs (-D): " << max_dur << endl;
    cout << "threshold short/long (-t): " << threshold << endl;

}

void Sym_Params::print_help()
{
    cout << "Help for simul" << endl;
    cout << "    -h  : this help" << endl;
    cout << "    -f  : file containing the task set (default = tset.txt) (see taskgen.py for more info)" << endl;
     cout << "    -p  : file containing the lists of processor (default = proc.txt) " << endl;
    cout << "    -o  : output file (default = tres.txt)" << endl;

    cout << "    -d  : min duration of cs (default = 5 (usec)) " << endl;
    cout << "    -D  : max duration of cs (default = 500 (usec)) " << endl;
    cout << "    -t  : threshold short/long (default = 50 (usec)) " << endl;
    cout << "    -r  : random seed (default = 12345) " << endl;
    cout << "    -x  : max tasks per group (default = 6) " << endl;
    cout << "    -y  : min tasks per group (default = 2) " << endl;
}




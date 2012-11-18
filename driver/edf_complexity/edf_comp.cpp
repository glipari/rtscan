#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <cmath>
#include <boost/program_options.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/error_of.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>

#include <common/generators.hpp>
#include <analysis/fp_response_time.hpp>

using namespace std;
using namespace Scan;

namespace po = boost::program_options;
namespace ac = boost::accumulators;

struct PlotElem {
    int col;
    int col2;
    std::string file;
    std::string label;
    std::string graphtype; // could be errorbars, lines, linespoints, etc.

    PlotElem() :
        col(2), col2(3),
        file("input.txt"),
        label(""),
        graphtype("lines") {}
    
    std::string fmt() {
        std::stringstream str;
        str << "\"" << file << "\" ";
        str << "using 1:" << col << ":" << col2 << " ";
        str << "w " << graphtype << " ";
        str << "t \"" << label << "\"" << endl;
        return str.str();
    }
};


/** By convention, the first column is the x axis */
struct GnuPlotGraph {
    std::string scriptfile;
    std::string outfile;
    std::string xaxis;
    std::string yaxis;
    std::string key_pos;
    std::string xrange;
    std::string yrange;
    std::string title;
    std::vector<PlotElem> plots;

    GnuPlotGraph() :
        scriptfile("gplot.gpl"),
        outfile("graph.eps"),
        xaxis("x"),
        yaxis("y"),
        key_pos("top right"),
        xrange(""), 
        yrange(""),
        title(""),
        plots() {}

    void write() {
        ofstream f(scriptfile);
        f << "set term postscript eps color font \"Times-Roman\" 24 lw 1.6" << endl;
        f << "set output \"" << (outfile) << "\"" << endl;
        f << "set xlabel \"" << xaxis << "\"" << endl;
        f << "set ylabel \"" << yaxis << "\"" << endl;
        if (title != "") f << "set title \"" << title << "\"" << endl;
        if (xrange != "") f << "set xrange " << xrange << endl;
        if (yrange != "") f << "set yrange " << yrange << endl;        
        f << "plot ";
        f << plots[0].fmt();
        for (int i=1; i<plots.size(); ++i) f << ", " << plots[i].fmt();
        f.close();
    }
};


/**
   This file is concerned with EDF scheduling on single processors,
   and Demand Bound Analysis.

   In DBF analysis for synchronous periodic task sets with constrained
   deadlines, the demand has to be checked for all deadlines in a
   certain interval (less than the hyperperiod). There are two ways to
   bound this interval: the busy interval (Spuri et al) and the
   interval bound by Baruah et al. and later by Ripoll et al.

   Which one of the two bounds is smaller, thus leading to less
   overhead?

   This little program answers the question. For each utilization, I
   generate a number of task sets (nsamples=100), each one with a
   number of task sets, with random utilizations, periods and
   deadlines. For each task set I compute both bounds. Then I count
   how many times the first interval is less than the second one. 

   I do this a number of times (nrep=100) for each utilization, and I
   compute the average and the std dev of this count. Finally, I do
   this for all utilization up to .99.

   The results are quite surprising!
*/
int main(int argc, char* argv[])
{
    double util_min = .5;    // starting from utilization equal to 0.5
    double util_max = .99;   // until 0.99 
    double util_step = 0.01; // the utilizazion step
    int ntasks = 3;          // number of tasks
    int nrep;                // number of repetitions
    int nsamples = 1000;     // number of samples
    string output_filename;  // output file name
    int p_min, p_max;        // periods (min, max)
    double dr_min, dr_max;   // deadline ration (min, max)
    string plot_title;

    // prepare command line options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message")
        ("umin,a", po::value<double>(&util_min)->default_value(.5), "minimum utilization")
        ("umax,b", po::value<double>(&util_max)->default_value(.999), "maximum utilization")
        ("ustep,s", po::value<double>(&util_step)->default_value(.01), "utilization step")
        ("tasks,n", po::value<int>(&ntasks)->default_value(3), "number of tasks")
        ("nsamples,m", po::value<int>(&nsamples)->default_value(100), "number of samples per point")
        ("nrep,y", po::value<int>(&nrep)->default_value(100), "number of repetitions")
        ("pmin,p", po::value<int>(&p_min)->default_value(100), "minimum task period")
        ("pmax,P", po::value<int>(&p_max)->default_value(10000), "maximum task period")
        ("dr_min,d", po::value<double>(&dr_min)->default_value(1), "minimum deadline ratio")
        ("dr_max,D", po::value<double>(&dr_max)->default_value(1), "mmaximum deadline ratio")
        ("output,o", po::value<std::string>(&output_filename)->default_value("output"), "output file name")
        ("title,t", po::value<std::string>(&plot_title)->default_value(""), "title of the plot")
        ;

    // parse command line
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))  {
        cout << desc << endl;
        exit(0);
    }

    cout << "umin = " << util_min << endl;
    cout << "umax = " << util_max << endl;
    cout << "ustep = " << util_step << endl;
    cout << "tasks = " << ntasks << endl;
    cout << "nsamples = " << nsamples << endl;
    cout << "nrep = " << nrep << endl;
    cout << "pmin = " << p_min << endl;
    cout << "pmax = " << p_max << endl;
    cout << "dr_min = " << dr_min << endl;
    cout << "dr_max = " << dr_max << endl;
    cout << "output file = " << output_filename << endl;
    cout << "title = " << plot_title << endl; 

    // init random seed
    GeneratorSingleton::init(12345);


    // open output file
    ofstream output(output_filename.c_str());
    
    // main cycle
    for (double u = util_min; u<= util_max; u+= util_step) {
        // statistics to be collected
        ac::accumulator_set<int, ac::stats<ac::tag::mean, ac::tag::variance(ac::lazy) > > allcount;
        
#pragma omp parallel for 
        for (int k = 0; k<nrep; k++) {
            int count = 0;  // number of times busy period > deadline limit
            for (int i = 0; i<nsamples; i++) {
                // used to generate utilizations
                vector<double> utils = UUniFast(ntasks, u);
                // generate periods
                std::uniform_int_distribution<int> period_dist(p_min, p_max);
                //std::lognormal_distribution<double> period_dist(p_min, p_max);

                // used to generate deadlines
                std::uniform_real_distribution<double> dline_dist(dr_min,dr_max);
                
                vector<Task> tasks;
                for (auto x : utils) {
                    int per = period_dist(RNDGEN());
                    double wcet = per * x;
                    int dline = per * dline_dist(RNDGEN());
                    if (dline < wcet) dline = wcet;
                    tasks.push_back(Task(wcet, dline, per));
                }
            
                // compute the lenght of the interval using the busy interval (does not depend on deadlines)
                double l_old = 0;
                double l1 = tasks[0].get_wcet();
                while (l1 > l_old) {
                    l_old = l1;
                    l1 = workload(tasks.begin(), tasks.end(), 0, l1);
                }

                // compute the lenght of the interval using the formula (does depend on deadlines)
                double l2 = 0;
                double s = 0;
                for (auto t : tasks) { 
                    l2 = max(l2, t.get_period() - t.get_dline());
                    s += max(0.0, t.get_period() - t.get_dline()) * (t.get_wcet() / t.get_period());
                }
                l2 = max(l2, s/(1-u));

                // count the number of times l1 < l2
                if (l1 < l2)
                    count++;
            }
            // collect statistics
#pragma omp critical 
            {
                allcount(count);
            }
        }
        
        // output statistics on terminal
        cout << "u = " << u << "\n" 
             << "     Count  = " << ac::mean(allcount) << ", " << sqrt(ac::variance(allcount)) << endl;

        // output on file
        output << u << ", " 
               << ac::mean(allcount) << ", "
               << sqrt(ac::variance(allcount)) << endl;

    }

    GnuPlotGraph gp;
    gp.outfile = output_filename + ".eps";
    gp.xaxis = "Utilization";
    gp.yaxis = "Count";
    gp.key_pos = "bottom left";
    gp.scriptfile = output_filename + ".gpl";
    stringstream ss;
    ss << "[0:" << (nsamples + (int)(nsamples/20)) <<"]";
    gp.yrange = ss.str();
    gp.xrange = "[0.3:1]";
    gp.title = plot_title;
    PlotElem pe;
    pe.file = output_filename;
    pe.graphtype = "errorbars ls 1";

    gp.plots.push_back(pe);    
    gp.write();

    string cmd("gnuplot ");
    cmd += output_filename + ".gpl";
    system(cmd.c_str());
} 

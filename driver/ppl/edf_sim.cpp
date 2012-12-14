#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
//#include <random>
#include <algorithm>
#include <cmath>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <analysis/task_utility.hpp>
#include <models/task_parser.hpp>
#include <models/fp_task.hpp>
#include "ppl_utils.hpp"

#include <boost/program_options.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/error_of.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>

namespace PPL = Parma_Polyhedra_Library;
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
        ofstream f(scriptfile.c_str());
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



int main(int argc, char* argv[])
{
    int ntasks = 3;          // number of tasks
    int nsamples = 1000;     // number of samples
    string output_filename;  // output file name
    int p_min, p_max;        // periods (min, max)
    int p_gcd;               // min gcd between any two periods
    double dr_min, dr_max;   // deadline ratio (min, max)
    double dr_step;          // deadline ratio (min, max)
    string plot_title;       // a string with the title of the plot

    // prepare command line options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message")
        ("tasks,n", po::value<int>(&ntasks)->default_value(3), "number of tasks")
        ("nsamples,m", po::value<int>(&nsamples)->default_value(100), "number of samples per point")
        ("pmin,p", po::value<int>(&p_min)->default_value(100), "minimum task period")
        ("pmax,P", po::value<int>(&p_max)->default_value(10000), "maximum task period")
        ("pgcd,g", po::value<int>(&p_gcd)->default_value(10), "minimum gcd between any two task periods")
        ("dr_min,d", po::value<double>(&dr_min)->default_value(.5), "minimum deadline ratio")
        ("dr_max,D", po::value<double>(&dr_max)->default_value(1), "mmaximum deadline ratio")
        ("dr_step,s", po::value<double>(&dr_step)->default_value(.05), "mmaximum deadline ratio")
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

    cout << "p_gcd = " << p_gcd << endl;
    cout << "p_max = " << p_max << endl;
    cout << "p_min = " << p_min << endl;

    //GeneratorSingleton::init(12345);
    // open output file
    ofstream output(output_filename.c_str());

    for (double dr=dr_min; dr<dr_max; dr+=dr_step) { 
        ac::accumulator_set<double, ac::stats<ac::tag::mean, ac::tag::variance(ac::lazy) > > allcount;
        for (int k=0; k<nsamples; ++k) {
            // extract random periods
            vector<FPTask> v;
            int dist = p_max/p_gcd - p_min/p_gcd;
            cout << "dist = " << dist << endl;
            // std::uniform_int_distribution<int> period_dist(p_min/p_gcd, p_max/p_gcd);
            for (int i=0; i<ntasks; ++i) {
                // int per = period_dist(RNDGEN()) * p_gcd;
                int per = (rand() % dist) * p_gcd + p_min;
                int dline = per * dr;
                v.push_back(FPTask(1, dline, per));
            }
            for (int i=0; i<ntasks; i++) cout << v[i] << endl;

            int h = compute_hyperperiod(v.begin(), v.end(), v.size());
            cout << "Hyperperiod: " << h << endl;
            
            // now build the constraint system
            PPL::C_Polyhedron poly(v.size());
            vector<string> vars;
            
            build_edf_base(v, poly, vars);
            build_edf_constraints(v, poly);
            
            PPL::Constraint_System full_cons_sys = poly.constraints();
            PPL::Constraint_System redu_cons_sys = poly.minimized_constraints();
            
            int s1 = how_many_constraints(full_cons_sys);    
            int s2 = how_many_constraints(redu_cons_sys);
            
            cout << "Number of constraints (full): " << s1 << endl;
            cout << "Memory (full): " << full_cons_sys.total_memory_in_bytes() << endl;
            cout << "Number of constraints (redu): " << s2 << endl;
            cout << "Memory (redu): " << redu_cons_sys.total_memory_in_bytes() << endl;
            
            using namespace PPL::IO_Operators;
            cout << poly << endl;
            
            allcount(double(s2-ntasks)/double(ntasks)); ///double(s1-ntasks));
            
        }
        // output statistics on terminal
        cout << "dr = " << dr << "\n" 
             << "     Count  = " << ac::mean(allcount) << ", " << sqrt(ac::variance(allcount)) << endl;

        // output on file
        output << dr << ", " 
               << ac::mean(allcount) << ", "
               << sqrt(ac::variance(allcount)) << endl;

    }

    GnuPlotGraph gp;
    gp.outfile = output_filename + ".eps";
    gp.xaxis = "Deadline / Period ratio";
    gp.yaxis = "Ratio ndlines/ntasks";
    gp.key_pos = "bottom left";
    gp.scriptfile = output_filename + ".gpl";
    gp.yrange = "[0:10]";
    gp.xrange = "[0:1]";
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

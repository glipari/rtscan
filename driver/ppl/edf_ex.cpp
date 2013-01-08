#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <analysis/task_utility.hpp>
#include <models/task_parser.hpp>
#include <common/interval.hpp>
#include "ppl_utils.hpp"

namespace PPL = Parma_Polyhedra_Library;
using namespace std;
using namespace Scan;

vector< IntervalType<int> > compute_idle_intervals(const Task &t, int h)
{
    int x = 0;
    int d = 0;
    vector< IntervalType<int> > intervals; 
    while (x < h) {
        d = get_next_deadline(t, x);
        x = get_next_arrival(t, x);
        IntervalType<int> l(d,x);
        if (!l.is_empty()) intervals.push_back(l);
    }
    return intervals;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename> " << endl;
        exit(-1);
    }
    string fname(argv[1]);
    ifstream input(fname.c_str());
    PropertyList sys;
    parse_properties(input, fname, sys);

    PrintPropertyVisitor vis;
    vis(sys);
    
    SysVisitor sv;
    sv(sys);

    for (vector<FPTask>::iterator i = sv.v.begin();
         i != sv.v.end(); ++i) 
        cout << *i << endl;
  
    int h = compute_hyperperiod(sv.v.begin(), sv.v.end(), sv.v.size());
    cout << "Hyperperiod: " << h << endl;
    
    // now build the constraint system
    PPL::C_Polyhedron poly(sv.v.size());
    vector<string> vars;

    build_edf_base(sv.v, poly, vars);
    build_edf_constraints(sv.v, poly);
    
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

    cout << "Computing idle intervals" << endl;

    vector< IntervalType<int> > intervals = compute_idle_intervals(sv.v[0], h);
    for (int i=1; i<sv.v.size(); i++) {
        vector< IntervalType<int> > newIntervals;
        vector< IntervalType<int> > inter2 = compute_idle_intervals(sv.v[i], h);
        for (int k=0; k<inter2.size(); k++) 
            interval_intersect_all(intervals.begin(), 
                                   intervals.end(), inter2[k], 
                                   back_inserter(newIntervals));
        
        intervals = newIntervals;
    }

    print_sequence(intervals.begin(), intervals.end(), cout);
    cout << endl;
}


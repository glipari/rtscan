#include <iostream>
#include <fstream>
#include <ppl.hh>
#include <boost/ref.hpp>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>
#include <analysis/hyperplane.hpp>

namespace PPL = Parma_Polyhedra_Library;
using namespace std;
using namespace Scan;

bool check_point_contains(const PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps, 
                          int xn, int xd, int yn, int yd)
{
    PPL::Variable x(0);
    PPL::Variable y(1);
    PPL::Constraint c1 = (xd*x == xn);
    PPL::Constraint c2 = (yd*y == yn);
    
    PPL::Pointset_Powerset<C_Polyhedron> ps_point(2, EMPTY);
    PPL::C_Polyhedron in1(2);
    in1.add_constraint(c1);
    in1.add_constraint(c2); 
    ps_point.add_disjunct(in1);

    return ps.contains(ps_point);
}


class SysVisitor : public boost::static_visitor<> {
public:
    std::vector<FPTask> v;

    SysVisitor() {}

    void operator()(const Scan::Property &p) {
        cerr << "ERROR: parsing a property at the sys level" << endl;
    }
    void operator()(const PropertyList &pl) {
        if (pl.type == "sys") 
            for (std::vector< PropertyList::Element >::const_iterator i = pl.children.begin();
                 i != pl.children.end(); i++) 
                boost::apply_visitor(*this, *i);
        else if (pl.type != "task") 
            cerr << "Found a property list which is not a task!" << endl;
        else 
            v.push_back(create_fp_task(pl));
    }
    
};


Pointset_Powerset<C_Polyhedron> create_powerset(std::vector<FPTask> &v)
{
    //sort_by_decreasing(v.begin(), v.end(), &FPTask::get_priority);

    const int ntasks = v.size(); 
    Pointset_Powerset<C_Polyhedron> final(ntasks, EMPTY);
    // the base polyhedron
    PPL::C_Polyhedron base(ntasks);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xx(i);
        PPL::Constraint cs_min = (xx >= 0);
        base.add_constraint(cs_min);
        PPL::Constraint cs_max = (xx <= (int)v[i].get_dline());
        base.add_constraint(cs_max);
    }
    
    std::vector<Task> v2;
    copy(v.begin(), v.end(), v2.begin());

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
    
    cout << "Sys parsed!" << endl;

    for (vector<FPTask>::iterator i = sv.v.begin();
         i != sv.v.end(); ++i) 
        cout << *i << endl;

    // tasks have been read, now create the Pointset_Powerset
    int ntasks = sv.v.size();
    
    PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(ntasks, EMPTY);
    PPL::C_Polyhedron base(ntasks);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xx(i);
        PPL::Constraint cs_min = (xx >= 0);
        base.add_constraint(cs_min);
        PPL::Constraint cs_max = (xx <= (int)sv.v[i].get_dline());
        base.add_constraint(cs_max);
    }
    
    double dline = sv.v[ntasks-1].get_dline();
    vector<double> points = compute_points(sv.v.begin(), sv.v.end()-1, dline);
    for (int k=0; k<points.size(); k++) {
        PPL::C_Polyhedron cp = base; 
        PPL::Linear_Expression le;
        for (int i=0; i<ntasks; i++) {
            PPL::Variable xx(i); 
            le += xx * ((int)compute_coeff(points[k], sv.v[i].get_period()));
        }
        PPL::Constraint cs = (le <= (int)points[k]);
        cp.add_constraint(cs);
        ps.add_disjunct(cp);
    }

    using namespace PPL::IO_Operators;
    cout << ps << endl;
}


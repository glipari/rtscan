#include "ppl_utils.hpp"

#include <analysis/hyperplane.hpp>

using namespace Scan;
using namespace std;

bool check_point_contains(const PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps, 
                          int xn, int xd, int yn, int yd)
{
    PPL::Variable x(0);
    PPL::Variable y(1);
    PPL::Constraint c1 = (xd*x == xn);
    PPL::Constraint c2 = (yd*y == yn);
    
    PPL::Pointset_Powerset<PPL::C_Polyhedron> ps_point(2, EMPTY);
    PPL::C_Polyhedron in1(2);
    in1.add_constraint(c1);
    in1.add_constraint(c2); 
    ps_point.add_disjunct(in1);

    return ps.contains(ps_point);
}


PPL::Pointset_Powerset<PPL::C_Polyhedron> build_hyperplanes_powerset(vector<FPTask> &v)
{
    int ntasks = v.size();

    PPL::Pointset_Powerset<PPL::C_Polyhedron> final_ps(ntasks, EMPTY);

    PPL::C_Polyhedron base(ntasks);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xx(i);
        PPL::Constraint cs_min = (xx >= 0);
        base.add_constraint(cs_min);
        PPL::Constraint cs_max = (xx <= (int)v[i].get_dline());
        base.add_constraint(cs_max);
    }
    
    final_ps.add_disjunct(base);
    for (int j=1; j<ntasks;j++) {
        PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(ntasks, EMPTY);
        
        double dline = v[j].get_dline();
        vector<double> points = compute_points(v.begin(), v.end()-(ntasks-j), dline);
        for (int k=0; k<points.size(); k++) {
            PPL::C_Polyhedron cp = base; 
            PPL::Linear_Expression le;
            for (int i=0; i<=j; i++) {
                PPL::Variable xx(i); 
                le += xx * ((int)compute_coeff(points[k], v[i].get_period()));
            }
            
            PPL::Constraint cs = (le <= (int)points[k]);
            cp.add_constraint(cs);
            ps.add_disjunct(cp);
        }
        // at this point, I have a ps. I have to intersect with the previous one
        final_ps.intersection_assign(ps);
    }
    return final_ps;
}

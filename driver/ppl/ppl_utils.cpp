#include <boost/math/common_factor.hpp>

#include "ppl_utils.hpp"


#include <analysis/hyperplane.hpp>

using namespace Scan;
using namespace std;

// bool check_point_contains(const PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps, 
//                           int xn, int xd, int yn, int yd)
// {
//     PPL::Variable x(0);
//     PPL::Variable y(1);
//     PPL::Constraint c1 = (xd*x == xn);
//     PPL::Constraint c2 = (yd*y == yn);
    
//     PPL::Pointset_Powerset<PPL::C_Polyhedron> ps_point(2, EMPTY);
//     PPL::C_Polyhedron in1(2);
//     in1.add_constraint(c1);
//     in1.add_constraint(c2); 
//     ps_point.add_disjunct(in1);

//     return ps.contains(ps_point);
// }

template<class IterInt, class IterTask>
vector< vector<int> > number_of_instances(IterInt pb, IterInt pe, IterTask tb, IterTask te) 
{
    vector< vector<int> > vect;
    for (IterInt p = pb; p!= pe; ++p) {
        vector<int> n;
        for (IterTask t = tb; t!=te; ++t) {
            n.push_back(ceil(*p/t->get_period()));
            // remember to add max_deadline! 
        }
        vect.push_back(n);
    }
    return vect;
}


PPL::Pointset_Powerset<PPL::C_Polyhedron> build_hyperplanes_powerset(vector<FPTask> &v, vector<string> &vars)
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
        vars.push_back(v[i].get_name() + ".wcet");
    }
    
    final_ps.add_disjunct(base);
    for (int j=1; j<ntasks;j++) {
        PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(ntasks, EMPTY);
        
        double dline = v[j].get_dline();
        vector<int> points = compute_points(v.begin(), v.end()-(ntasks-j), dline);
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

template<class Iter>
int compute_hyperperiod(Iter b, Iter e)
{
    int l = 1;
    for (Iter i = b; i!=e; ++i) l =  boost::math::lcm<int>(l, i->get_period());
    return l;
}

PPL::Pointset_Powerset<PPL::C_Polyhedron> build_general_sensitivity(vector<FPTask> &v, vector<string> &vars)
{
    int ntasks = v.size();
    int nvars = ntasks * 3;

    PPL::Pointset_Powerset<PPL::C_Polyhedron> final_ps(nvars, EMPTY);
    
    PPL::C_Polyhedron base(ntasks);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xw(3*i);
        PPL::Constraint cs_min = (xw >= 0);
        base.add_constraint(cs_min);
        PPL::Constraint cs_max = (xw <= (int)v[i].get_period());
        base.add_constraint(cs_max);
        vars.push_back(v[i].get_name() + ".wcet");

        PPL::Variable xd(3*i+1);
        PPL::Constraint dd_min = (xd >= 0);
        base.add_constraint(dd_min);
        PPL::Constraint dd_max = (xd <= int(v[i].get_dline()));
        base.add_constraint(dd_max);
        vars.push_back(v[i].get_name() + ".dline");

        PPL::Variable xj(3*i+2);
        PPL::Constraint j_min = (xj >= 0);
        base.add_constraint(j_min);
        PPL::Constraint j_max = (xj <= int(v[i].get_dline()));
        base.add_constraint(j_max);
        vars.push_back(v[i].get_name() + ".jitter");
    }
    final_ps.add_disjunct(base);
    
    // compute hyperperiod
    int hyper = compute_hyperperiod(v.begin(), v.end());
    
    for (int i=1; i<ntasks; i++) {
        // now, for every h up to H/T_i
        int h_max = hyper / v[i].get_period();
        for (int h = 1; h<h_max; ++h) {
            // now we have to compute all vectors of n
            vector<int> pp = compute_points(v.begin(), v.end() - (ntasks-i), v[i].get_dline());
            sort(pp.begin(), pp.end());
            vector< vector<int> > myn = 
                number_of_instances(pp.begin(), pp.end(), v.begin(), v.end() - (ntasks - i));

            // now, for every n, we must write one set of equations
            
        }
    }

}


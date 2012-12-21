#include "ppl_utils.hpp"

#include <analysis/hyperplane.hpp>
#include <analysis/task_utility.hpp>

#include <common/string_utils.hpp>

using namespace Scan;
using namespace std;

template<class IterInt, class IterTask>
vector< vector<int> > number_of_instances(IterInt pb, IterInt pe, IterTask tb, IterTask te) 
{
    vector< vector<int> > vect;
    for (IterInt p = pb; p!= pe; ++p) {
        vector<int> n;
        for (IterTask t = tb; t!=te; ++t) {
            int x = *p / t->get_period();
            if (*p % t->get_period()) x++; 
            n.push_back(x);
            // remember to add max_deadline! 
        }
        vect.push_back(n);
    }
    return vect;
}

ConstraintsSystem::ConstraintsSystem(int n) :
    poly(n,EMPTY), vars(n)
{
}

ConstraintsSystem build_hyperplanes_powerset(std::vector<Scan::FPTask> &v)
{
    int ntasks = v.size();

    ConstraintsSystem sys(ntasks);

    PPL::C_Polyhedron base(ntasks);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xx(i);
        PPL::Constraint cs_min = (xx >= 0);
        base.add_constraint(cs_min);
        PPL::Constraint cs_max = (xx <= (int)v[i].get_dline());
        base.add_constraint(cs_max);
        sys.vars[i] = (v[i].get_name() + ".wcet");
    }
    
    sys.poly.add_disjunct(base);
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
        sys.poly.intersection_assign(ps);
    }
    return sys;
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
void print_vector(Iter b, Iter e)
{
    cout << "[";
    for (Iter i=b; i!=e;++i) { 
        cout << *i;
        if (i+1 != e) cout << ", ";
    } 
    cout << "]";
}

ConstraintsSystem build_general_sensitivity(vector<FPTask> &v)
{
    int ntasks = v.size();
    int nvars = ntasks * 3;
    ConstraintsSystem sys(nvars);

    PPL::C_Polyhedron base(nvars);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xw(3*i);

        PPL::Constraint cs_min = (xw >= 0);
        base.add_constraint(cs_min);

        PPL::Constraint cs_max = (xw <= (int)v[i].get_period());
        base.add_constraint(cs_max);
        sys.vars[3*i] = v[i].get_name() + ".wcet";

        PPL::Variable xd(3*i+1);
        PPL::Constraint dd_min = (xd >= 0);
        base.add_constraint(dd_min);
        PPL::Constraint dd_max = (xd <= int(v[i].get_dline()));
        base.add_constraint(dd_max);
        sys.vars[3*i+1] = v[i].get_name() + ".dline";

        PPL::Variable xj(3*i+2);
        PPL::Constraint j_min = (xj >= 0);
        base.add_constraint(j_min);
        PPL::Constraint j_max = (xj <= int(v[i].get_dline()));
        base.add_constraint(j_max);
        sys.vars[3*i+2] = v[i].get_name() + ".jitter";
    }
    sys.poly.add_disjunct(base);
    
    for (int i=1; i<ntasks; i++) {
        // compute hyperperiod
        int hyper = compute_hyperperiod(v.begin(), v.end(), i+1);
        // now, for every h up to H/T_i
        int h_max = 1;
        if (v[i].get_dline() > v[i].get_period()) 
            h_max = hyper / v[i].get_period();
        for (int h = 1; h<=h_max; ++h) {
            // now we have to compute all vectors of n
            vector<int> pp = compute_all_points(v.begin(), 
                                                v.end() - (ntasks-i), 
                                                (h-1)*v[i].get_period() + v[i].get_dline());
            
            vector< vector<int> > myn = 
                number_of_instances(pp.begin(), pp.end(), 
                                    v.begin(), v.end() - (ntasks-i));

            cout << "Task " << v[i].get_name() << endl;
            cout << "Instance h = " << h << endl;
            cout << "Points: ";
            print_vector(pp.begin(), pp.end());
            cout << endl;
            cout << "Vector n: ";
            for (unsigned p=0;p<myn.size(); p++)  {
                print_vector(myn[p].begin(), myn[p].end());
                cout << ";";
            }
            cout << endl;
            
            using namespace PPL::IO_Operators;
            cout << "Now preparing the pointset" << endl;

            PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(nvars, EMPTY);
            // now, for every n, we must write one set of equations
            for (unsigned n=0; n<myn.size(); ++n) {
                cout << "Point: ";
                print_vector(myn[n].begin(), myn[n].end());
                cout << endl;

                PPL::C_Polyhedron cp = base;
                cout << "Copied" << "   i = " << i << endl;
                for (unsigned k=0; k<i; ++k) {
                    PPL::Linear_Expression le;
                    for (int j=0; j<i; j++) {
                        PPL::Variable xx(3*j); 
                        le += xx * myn[n][j];
                    }
                    PPL::Variable xx(3*i); 
                    PPL::Variable xj(3*k+2);
                    le += h * xx;
                    le += xj;
                    PPL::Constraint cs = (le <= myn[n][k]*v[k].get_period());
                    cp.add_constraint(cs);
                }
                
                PPL::Linear_Expression le;
                for (unsigned j=0; j<i; j++) {
                    PPL::Variable xx(3*j); 
                    le += xx * myn[n][j];
                }
                PPL::Variable xx(3*i);
                PPL::Variable xd(3*i+1);
                PPL::Variable xj(3*i+2);
                le += h * xx;
                le += -xd+xj;  // to be tested
                PPL::Constraint cs = (le <= (h-1)*v[i].get_period());
                cp.add_constraint(cs);

                //cout << "Constraint set completed: " << cp << endl;
                cout << "Constraint set completed: " << endl;

                ps.add_disjunct(cp);
                //ps.omega_reduce();
            }
            cout << "Now all constraint set have been prepared, intesecting...";
            cout.flush();
            sys.poly.intersection_assign(ps);
            cout << "... completed!" << endl;
        }
    }
    return sys;
}



// PPL::Pointset_Powerset<PPL::C_Polyhedron> build_general_sensitivity(vector<FPTask> &v, vector<string> &vars)
// {
//     int ntasks = v.size();
//     int nvars = ntasks * 3;

//     PPL::Pointset_Powerset<PPL::C_Polyhedron> final_ps(nvars, EMPTY);
    
//     PPL::C_Polyhedron base(nvars);
//     for (int i=0;i<ntasks;i++) {
//         PPL::Variable xw(3*i);
//         PPL::Constraint cs_min = (xw >= 0);
//         base.add_constraint(cs_min);
//         PPL::Constraint cs_max = (xw <= (int)v[i].get_period());
//         base.add_constraint(cs_max);
//         vars.push_back(v[i].get_name() + ".wcet");

//         PPL::Variable xd(3*i+1);
//         PPL::Constraint dd_min = (xd >= 0);
//         base.add_constraint(dd_min);
//         PPL::Constraint dd_max = (xd <= int(v[i].get_dline()));
//         base.add_constraint(dd_max);
//         vars.push_back(v[i].get_name() + ".dline");

//         PPL::Variable xj(3*i+2);
//         PPL::Constraint j_min = (xj >= 0);
//         base.add_constraint(j_min);
//         PPL::Constraint j_max = (xj <= int(v[i].get_dline()));
//         base.add_constraint(j_max);
//         vars.push_back(v[i].get_name() + ".jitter");
//     }
//     final_ps.add_disjunct(base);
    
//     for (int i=1; i<ntasks; i++) {
//         // compute hyperperiod
//         int hyper = compute_hyperperiod(v.begin(), v.end(), i+1);
//         // now, for every h up to H/T_i
//         int h_max = 1;
//         if (v[i].get_dline() > v[i].get_period()) h_max = hyper / v[i].get_period();
//         for (int h = 1; h<=h_max; ++h) {
//             // now we have to compute all vectors of n
//             vector<int> pp = compute_all_points(v.begin(), v.begin()+i+1, (h-1)*v[i].get_period() + v[i].get_dline());
//             vector< vector<int> > myn = 
//                 number_of_instances(pp.begin(), pp.end(), v.begin(), v.end() - (ntasks-i));

//             cout << "Task " << v[i].get_name() << endl;
//             cout << "Instance h = " << h << endl;
//             cout << "Points: ";
//             print_vector(pp.begin(), pp.end());
//             cout << endl;
//             cout << "Vector n: ";
//             for (unsigned p=0;p<myn.size(); p++)  {
//                 print_vector(myn[p].begin(), myn[p].end());
//                 cout << ";";
//             }
//             cout << endl;
            
//             using namespace PPL::IO_Operators;
//             cout << "Now preparing the pointset" << endl;

//             PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(nvars, EMPTY);
//             // now, for every n, we must write one set of equations
//             for (unsigned n=0; n<myn.size(); ++n) {
//                 cout << "Point: ";
//                 print_vector(myn[n].begin(), myn[n].end());
//                 cout << endl;

//                 PPL::C_Polyhedron cp = base;
//                 cout << "Copied" << "   i = " << i << endl;
//                 for (unsigned k=0; k<i; ++k) {
//                     PPL::Linear_Expression le;
//                     for (int j=0; j<i; j++) {
//                         PPL::Variable xx(3*j); 
//                         le += xx * myn[n][j];
//                     }
//                     PPL::Variable xx(3*i); 
//                     PPL::Variable xj(3*k+2);
//                     le += h * xx;
//                     le += xj;
//                     PPL::Constraint cs = (le <= myn[n][k]*v[k].get_period());
//                     cp.add_constraint(cs);
//                     //cp.omega_reduce();
//                 }
                
//                 PPL::Linear_Expression le;
//                 for (unsigned j=0; j<i; j++) {
//                     PPL::Variable xx(3*j); 
//                     le += xx * myn[n][j];
//                 }

//                 PPL::Variable xx(3*i);
//                 PPL::Variable xd(3*i+1);
//                 PPL::Variable xj(3*i+2);
//                 le += h * xx;
//                 le += -xd+xj;
//                 PPL::Constraint cs = (le <= (h-1)*v[i].get_period());
//                 cp.add_constraint(cs);
//                 //                cp.omega_reduce();

//                 cout << "Constraint set: " << cp << endl;
                
//                 ps.add_disjunct(cp);
//                 //ps.omega_reduce();
//             }
//             cout << "Now all constraint set have been prepared, intesecting...";
//             cout.flush();
//             final_ps.intersection_assign(ps);
//             final_ps.omega_reduce();
//             cout << "... completed!" << endl;

//             cout << "Partial pointset: " << final_ps << endl;
//         }
//     }
//     return final_ps;
// }


void build_edf_base(const vector<FPTask> &v, PPL::C_Polyhedron &poly, vector<string> &vars)
{
    for (int i=0;i<v.size();i++) {
        PPL::Variable xx(i);
        PPL::Constraint cs_min = (xx >= 0);
        poly.add_constraint(cs_min);
        PPL::Constraint cs_max = (xx <= (int)v[i].get_dline());
        poly.add_constraint(cs_max);
        vars.push_back(v[i].get_name() + ".wcet");
    }
}


void build_edf_constraints(const vector<FPTask> &v, PPL::C_Polyhedron &poly)
{
    vector<int> dlines = compute_all_deadlines(v.begin(), v.end());
    for (int i=0; i<dlines.size(); ++i) {
        PPL::Linear_Expression le;
        for (int j=0; j<v.size(); ++j) {
            PPL::Variable xx(j);
            le += xx * get_num_contained_instances(v[j], 0, dlines[i]);
        }
        PPL::Constraint cs = (le <= dlines[i]);
        poly.add_constraint(cs);
    }
}

int how_many_constraints(const PPL::Constraint_System &cs)
{
    int s1 = 0;
    for (PPL::Constraint_System::const_iterator k = cs.begin();
         k != cs.end(); ++k) s1++;
    return s1;
}



using namespace StrUtils;


/** returns the index corresponding to the position in the vector where vname is found, or -1 if 
    vname is not found */
int get_index(const std::vector<string> &var_names, const std::string &vname)
{
    for (unsigned i=0; i<var_names.size(); ++i) 
        if (var_names[i] == vname) return i;
    return -1;
}

/** find the task with a certain name */
int find_task(const std::vector<Scan::FPTask> &tset, const std::string &name)
{
    for (unsigned i=0; i<tset.size(); i++) 
        if (tset[i].get_name() == name) return i;
    return -1;
}

/** 
    Given a task, returns the property with name in var.

    TODO: generalize this function!
 */
double get_value_from_task(const FPTask &task, const std::string &var)
{
    if (var == "wcet") return task.get_wcet();
    else if (var == "period") return task.get_period();
    else if (var == "dline") return task.get_dline();
    else if (var == "jitter") return task.get_jitter();
    else if (var == "offset") return task.get_offset();
    return -1;
}


/**
   given the constraints in ps;
   and given the names of the variables in var_names (in the form "taskname.propname");
   
   1) sets all variables to their values, except for variable var, 
   thus obtaining a constraint set in one single variable

   2) computes max and minimum admissible values for that variable. 
 */
void ConstraintsSystem::do_sensitivity(// PPL::Pointset_Powerset<PPL::C_Polyhedron> ps, 
                    // const std::vector<string> &var_names,
                    const std::vector<Scan::FPTask> &tasks,
                    const std::string &var) 
{
    int k = get_index(vars, var);   // we do sensitivity on the k variable
    if (k == -1) throw("Variable not found");
    for (int i=0; i<vars.size(); i++)  {
        if (i == k) continue;
        vector<string> ss = split(vars[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "Task not found!";
        double v = get_value_from_task(tasks[ti], ss[1]);

        Variable xx(i);
        Congruence cg = ((xx %= int(v)) / 0); 
        poly.refine_with_congruence(cg);
    }
    Variable xx(k);
    Linear_Expression le;
    le += (xx);
    Coefficient mn;
    Coefficient md;
    bool is_included;
    poly.maximize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Maximum value for " << var << ": " << mn << "/" <<  md << endl;

    poly.minimize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Minimum value for " << var << ": " << mn << "/" <<  md << endl;
}

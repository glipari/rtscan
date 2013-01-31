#include "ppl_utils.hpp"

#include <analysis/hyperplane.hpp>
#include <analysis/task_utility.hpp>

#include <common/string_utils.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace Scan;
using namespace std;
using namespace PPL::IO_Operators;

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

ConstraintsSystem::ConstraintsSystem(int n, bool universe) :
	poly(n, UNIVERSE), vars(n)
{
}

ConstraintsSystem build_hyperplanes_powerset2(std::vector<Scan::FPTask> &v)
{
    int ntasks = v.size();

    ConstraintsSystem sys(ntasks);

    PPL::C_Polyhedron base(ntasks);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xx(v[i].get_id());
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
                PPL::Variable xx(v[i].get_id()); 
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

/* A set of help and refined methods for np_build_hyperplanes_powerset */
template<class Iter> static int compute_max_blocking_time(Iter b, Iter e);
static void constraints_of_np_job_i_k(const std::vector<Scan::FPTask> &v,
			const PPL::C_Polyhedron &base,
			const int ntasks, const int i, const int k,
			PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps_i);
static void constraints_of_np_task_i(const std::vector<Scan::FPTask> &v,
				const PPL::C_Polyhedron &base,
				const int ntasks, int i,
				PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps_i);

ConstraintsSystem np_build_hyperplanes_powerset(std::vector<Scan::FPTask> &v)
{
	int ntasks = v.size();
	
	ConstraintsSystem sys(ntasks);
		PPL::Variable yy(1.5);
		PPL::Variable xx(3.6);

	/**
	 * the basic constraint on each wcet that 0 <= C_i <= min(T_{i}, D_{i})
	 **/
	PPL::C_Polyhedron base(ntasks);
	for( int i = 0; i < ntasks; i++) {
		PPL::Variable xx(i);
		PPL::Constraint cs_min = (xx >= 0);
		base.add_constraint(cs_min);
		int tmp = min((int)(v[i].get_dline()), (int)(v[i].get_period()));
		PPL::Constraint cs_max = (xx <= tmp); 
		base.add_constraint(cs_max);
		sys.vars[i] = (v[i].get_name() + ".wcet");
	}

	sys.poly.add_disjunct(base);
	for( int i = 0; i < ntasks; i++) {
		/* The final set of constraints for task i */			
		PPL::Pointset_Powerset<PPL::C_Polyhedron> ps_i(ntasks, EMPTY);
		ps_i.add_disjunct(base);
		
		constraints_of_np_task_i(v, base, ntasks, i, ps_i);

		sys.poly.intersection_assign(ps_i);
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

template<class IterInt, class IterTask>
vector< vector<int> > number_of_instances_pline(IterInt pb, IterInt pe, IterTask tb, IterTask te) 
{
    vector< vector<int> > vect;
    for (IterInt p = pb; p!= pe; ++p) {
        vector<int> n;
        for (IterTask t = tb; t!=te; ++t) {
	    if( t->get_pipeline_tag() == te->get_pipeline_tag() && t->get_pipeline_tag() >= 0) {
	    	int ceil = 0;
		if( t->get_dline() > t->get_period()) {
			ceil = (t->get_dline() - t->get_period())/t->get_period();
			if( ((int)(t->get_dline() - t->get_period()))%(int)(t->get_period()) != 0)
				ceil += 1; 
		}
		int x = *p / t->get_period();
		if (*p % t->get_period()) x++;
		if( x > ceil) x = ceil; 
		n.push_back(x);
		continue;
 	    }	
            int x = *p / t->get_period();
            if (*p % t->get_period()) x++; 
            n.push_back(x);
            // remember to add max_deadline! 
        }
	bool pushed = false;
	for( int ii = 0; ii < vect.size(); ii++) {
		bool flag = true;
		for( int jj = 0; jj < vect[ii].size(); jj++)
			if( n[jj] != vect[ii][jj]) {
				flag = false;
				break;
			}
        	if( flag) { pushed = true; break; }
	}
        if( !pushed) vect.push_back(n);
    }
    return vect;
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
//cout<<sys.poly<<endl;
    /// why from i = 1? for (int i=1; i<ntasks; i++) {
    for (int i=0; i<ntasks; i++) {
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
			if(myn[n][k] != 0) {
                    PPL::Constraint cs = (le <= myn[n][k]*v[k].get_period());
                    cp.add_constraint(cs);	}
                }
                
                PPL::Linear_Expression le;
                for (unsigned j=0; j<i; j++) {
			if(myn[n][j] != 0) {
                    PPL::Variable xx(3*j); 
                    le += xx * myn[n][j];}
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
	    if( !ps.empty())
            sys.poly.intersection_assign(ps);
            cout << "... completed!" << endl;
        }
    }
    return sys;
}

using namespace StrUtils;

int get_index(const std::vector<string> &var_names, const std::string &vname);
int find_task(const std::vector<Scan::FPTask> &tset, const std::string &name);
double get_value_from_task(const FPTask &task, const std::string &var);

ConstraintsSystem build_general_sensitivity_pline(vector<FPTask> &v, vector<string> &vars_list)
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
	vector<string> backup = sys.vars;
	PPL::Variables_Set vars_;
	for(int i = 0; i < sys.vars.size(); i++) { 
		if( get_index(vars_list, sys.vars[i]) != -1) continue;
		
		vector<string> ss = split(sys.vars[i], ".");
		int ti = find_task(v, ss[0]);
		if(v[ti].get_pipeline_tag() == -1 || ss[1].compare("wcet") == 0 ) {
			double v_ = get_value_from_task(v[ti], ss[1]);

			Variable xx(i);
			Congruence cg = ((xx %= int(v_)) / 0);
			sys.poly.refine_with_congruence(cg);
			vars_.insert(xx);
			backup.erase(backup.begin() + get_index(backup, sys.vars[i]));
		}
//		} else if(  ss[1].compare("wcet") == 0) {
//			double v_ = get_value_from_task(v[ti], ss[1]);
//
//			Variable xx(i);
//			Congruence cg = ((xx %= int(v_)) / 0);
//			sys.poly.refine_with_congruence(cg);
//			vars_.insert(xx);
//		}
        }
	sys.poly.remove_space_dimensions(vars_);
    /// why from i = 1? for (int i=1; i<ntasks; i++) {
    for (int i=0; i<ntasks; i++) {
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

	    sort(pp.begin(), pp.end());
	    pp.erase( unique( pp.begin(), pp.end() ), pp.end());	    

            vector< vector<int> > myn = 
                number_of_instances_pline(pp.begin(), pp.end(), 
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
			if(myn[n][k] != 0) {
                    PPL::Constraint cs = (le <= myn[n][k]*v[k].get_period());
                    cp.add_constraint(cs);	}
                }
                
                PPL::Linear_Expression le;
                for (unsigned j=0; j<i; j++) {
			if(myn[n][j] != 0) {
                    PPL::Variable xx(3*j); 
                    le += xx * myn[n][j];}
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
		//if( !check_containment(cp, ps))
		ps.add_disjunct(cp);
//                ps.omega_reduce();
            }
            cout << "Now all constraint set have been prepared, intesecting...";
            cout.flush();
	    if( !ps.empty())
{
	    // Here, we must insantiate variables not in the vars-list 
	PPL::Variables_Set vars__;
	for(int i = 0; i < sys.vars.size(); i++) { // it = sys.vars.begin(); it != sys.vars.end(); it++) {
		if( get_index(vars_list, sys.vars[i]) != -1) continue;
		vector<string> ss = split(sys.vars[i], ".");
		int ti = find_task(v, ss[0]);
		if( v[ti].get_pipeline_tag() == -1 || ss[1].compare("wcet")==0) {
	                double v_ = get_value_from_task(v[ti], ss[1]);

                	Variable xx(i);
                	Congruence cg = ((xx %= int(v_)) / 0);
                	ps.refine_with_congruence(cg);
		        vars__.insert(xx);
        	}
//		else if( ss[1].compare("wcet") == 0) {
//	                double v_ = get_value_from_task(v[ti], ss[1]);
//
//        	        Variable xx(i);
//                	Congruence cg = ((xx %= int(v_)) / 0);
//                	ps.refine_with_congruence(cg);
//		        vars__.insert(xx);
//		}
        }
	ps.remove_space_dimensions(vars__);
        sys.poly.intersection_assign(ps);
} 
           cout << "... completed!" << endl;
        }
    }
	sys.vars=backup;
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
    PPL::Pointset_Powerset<PPL::C_Polyhedron> copied(poly);

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
        copied.refine_with_congruence(cg);
    }
    Variable xx(k);
    Linear_Expression le;
    le += (xx);
    Coefficient mn;
    Coefficient md;
    bool is_included;
    copied.maximize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Maximum value for " << var << ": " << mn << "/" <<  md << endl;

    copied.minimize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Minimum value for " << var << ": " << mn << "/" <<  md << endl;
}

void ConstraintsSystem::do_sensitivity2(
                    const std::vector<Scan::FPTask> &tasks,
                    const std::string &var1, const std::string &var2, const std::string &fname) 
{
    ofstream output;
    output.open(fname.c_str()); 
    PPL::Pointset_Powerset<PPL::C_Polyhedron> copied(poly);
    int k1 = get_index(vars, var1);   
    if (k1 == -1) throw("Variable not found");
    int k2 = get_index(vars, var2);   
    if (k2 == -1) throw("Variable not found");
    PPL::Variables_Set vars_;
    for (int i=0; i<vars.size(); i++)  {
        if (i == k1 || i == k2) continue;
	//Variable xx(i);
	//vars_.insert(xx);
        vector<string> ss = split(vars[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "Task not found!";
	/**
	    If tasks[ti] is not in a pipeline, we treat it the same
	    as in ''do_sensitivity''; that is, repling jitter and wcet
	    with their minimum values and dline with its maximum value.
	 */
	if( tasks[ti].get_pipeline_pos() == 0) {
		double v = get_value_from_task(tasks[ti], ss[1]);

		Variable xx(i);
		Congruence cg = ((xx %= int(v)) / 0); 
		copied.refine_with_congruence(cg);
	}
	/**
	    If it is a pipeline task, we can only insantiate its wcet
	    as 1 (the minimum value) since dline and jitter for a 
	    pipeline task are considered as internal varibles.
	 */ 
	else if( ss[1].compare("wcet") == 0) {
		double v = get_value_from_task(tasks[ti], ss[1]);

		Variable xx(i);
		Congruence cg = ((xx %= int(v)) / 0); 
		copied.refine_with_congruence(cg);
	}

   }
   for (int i=0; i<vars.size(); i++)  {
        if (i == k1 || i == k2) continue;
	Variable xx(i);
	vars_.insert(xx);
   }
   copied.remove_space_dimensions(vars_);
   cout<<"System only with variables "<<vars[k1]<<" and " <<vars[k2]<<" : "<<copied<<endl<<endl;
for( PPL::Pointset_Powerset<PPL::C_Polyhedron>::iterator i = copied.begin(); i != copied.end(); i++) {
		PPL::C_Polyhedron cp = i->pointset();
		PPL::Generator_System gs = cp.generators();
		cout<<gs<<endl;
                PPL::Constraint_System csi = i->pointset().constraints();
                for(PPL::Constraint_System::const_iterator j = csi.begin(); j != csi.end(); j++) {
                        output<<*j<<endl;
			
		}
		output<<endl;
    }

//   if( k1 < k2) {
	cout<<'A'<<": "<<vars[k1]<<endl;
	cout<<'B'<<": "<<vars[k2]<<endl;
	//output<<'A'<<": "<<vars[k1]<<endl;
	//output<<'B'<<": "<<vars[k2]<<endl;
//   } else {
//	cout<<'A'<<": "<<vars[k2]<<endl;
//	cout<<'B'<<": "<<vars[k1]<<endl;
	//output<<'A'<<": "<<vars[k2]<<endl;
	//output<<'B'<<": "<<vars[k1]<<endl;
  // }
}

void ConstraintsSystem::do_sensitivity3(
                    const std::vector<Scan::FPTask> &tasks,
                    const std::string &var)
{
    PPL::Pointset_Powerset<PPL::C_Polyhedron> copied(poly);
    int k = get_index(vars, var);   // we do sensitivity on the k variable
    if (k == -1) throw("Variable not found");
    for (int i=0; i<vars.size(); i++)  {
        if (i == k) continue;

        vector<string> ss = split(vars[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "Task not found!";
	if( tasks[ti].get_pipeline_pos() == 0) {
		double v = get_value_from_task(tasks[ti], ss[1]);

		Variable xx(i);
		Congruence cg = ((xx %= int(v)) / 0); 
		copied.refine_with_congruence(cg);
	}
	else if( ss[1].compare("wcet") == 0) {
		double v = get_value_from_task(tasks[ti], ss[1]);

		Variable xx(i);
		Congruence cg = ((xx %= int(v)) / 0); 
		copied.refine_with_congruence(cg);
	}

    }
	
    PPL::Variables_Set vars_;
    for (int i=0; i<vars.size(); i++)  {
        if (i == k) continue;
	Variable xx(i);
	vars_.insert(xx);
    }
    copied.remove_space_dimensions(vars_);
    cout<<copied<<endl;
    cout<<"A : " << var<<endl;
}

void ConstraintsSystem::do_sensitivity2(
                    const std::vector<Scan::FPTask> &tasks,
                    const std::string &var)
{
    PPL::Pointset_Powerset<PPL::C_Polyhedron> copied(poly);
//cout<<copied<<endl<<endl;	
    int k = get_index(vars, var);   // we do sensitivity on the k variable
    if (k == -1) throw("Variable not found");
//    int ii=-1;
//    int ki;
    for (int i=0; i<vars.size(); i++)  {
        if (i == k) continue;

/*        vector<string> ss = split(vars[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "Task not found!";
	// if the variable is in the vars-list or it is the "wcet" of a pipeline task
	if (  get_index(vars_list, vars[i]) != -1 || (tasks[ti].get_pipeline_tag() != -1 && ss[1].compare("wcet") == 0 ))
		ii ++;
	else continue;
*/	/**
	    If tasks[ti] is not in a pipeline, we treat it the same
	    as in ''do_sensitivity''; that is, repling jitter and wcet
	    with their minimum values and dline with its maximum value.
	 */
        vector<string> ss = split(vars[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "Task not found!";
	if( tasks[ti].get_pipeline_pos() == 0) {
		double v = get_value_from_task(tasks[ti], ss[1]);

		Variable xx(i);
		Congruence cg = ((xx %= int(v)) / 0); 
		copied.refine_with_congruence(cg);
	}
	/**
	    If it is a pipeline task, we can only insantiate its wcet
	    as 1 (the minimum value) since dline and jitter for a 
	    pipeline task are considered as internal varibles.
	 */ 
	else if( ss[1].compare("wcet") == 0) {
		double v = get_value_from_task(tasks[ti], ss[1]);

		Variable xx(i);
		Congruence cg = ((xx %= int(v)) / 0); 
		copied.refine_with_congruence(cg);
	}

    }
	
    Variable xx(k);
    Linear_Expression le;
    le += (xx);
    Coefficient mn;
    Coefficient md;
    bool is_included;
    bool res;
//std::cout<<copied<<endl;

    res=copied.maximize(le, mn, md, is_included);
//std::cout<<copied<<endl;
    // I should convert mn and md into a single double
    cout << "Upper bound value for " << var << ": " << mn << "/" <<  md << endl;

    res=copied.minimize(le, mn, md, is_included);
    // I should convert mn and md into a single double
    cout << "Lower bound value for " << var << ": " << mn << "/" <<  md << endl;
}

/* A helper function for np_build_hyperplanes_powerset */
template<class Iter>
static int compute_max_blocking_time(Iter b, Iter e)
{
	int B_i = 0;
	int tmp;
	for (Iter i=b; i != e; i++) {
		tmp = (int)(min(i->get_period(), (int)(i->get_dline())));// -1);
		if( B_i < tmp)
			B_i = tmp;
	}

	return B_i;
}

/**
	The same function to compute maximum blocking time of a non-preemptive 
	that (naively) considers pipeline's existence.
 */
template<class Iter>
static int compute_max_blocking_time_pline(Iter b, Iter e)
{
	int B_i = 0;
	int tmp;
	for (Iter i=b+1; i != e; i++) {
		
		if (i->get_pipeline_tag() == b->get_pipeline_tag() && i->get_pipeline_tag() >= 0) {
			tmp = 0;
			if ( i->get_dline() > i->get_period()) 
				tmp = min((int)(i->get_period()), (int)(i->get_dline() - i->get_period()));
		}
		else tmp = (int)(min(i->get_period(), (int)(i->get_dline())));// -1);
		if( B_i < tmp)
			B_i = tmp;
	}

	return B_i;
}

static void
constraints_of_np_job_i_k(const std::vector<Scan::FPTask> &v,
			const PPL::C_Polyhedron &base,
			const int ntasks, const int i, const int k,
			PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps_i)
{
	vector< vector<int> > points;
	vector<int> pp = compute_all_points(v.begin(), v.end() - (ntasks - i),
						k*v[i].get_dline());
	points = number_of_instances(pp.begin(), pp.end(),
					v.begin(), v.end() - (ntasks - i));

	/* Constraints for job k of task i */
	PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(ntasks, EMPTY);
	
	for(unsigned n = 0; n < points.size(); n++) {
		PPL::C_Polyhedron cp = base;

		for( unsigned nn = 0; nn < i; nn++) {
			PPL::Linear_Expression le;
			
			PPL::Variable xx(i);
			le += xx*(k-1) + v[nn].get_jitter();
			for( int j = 0; j < i; j++) {
				PPL::Variable xx(j);
				le += xx * points[n][j];
			}
			if( i == ntasks-1) {
				PPL::Constraint cs = ( le <= 
					points[n][nn]*v[nn].get_period()-1);

				cp.add_constraint(cs);
			}
			for (unsigned m = i + 1; m < ntasks; m++) {
				PPL::Linear_Expression le1 = le;
				PPL::Variable xx(m);
				le1 += xx - 1;
				PPL::Constraint cs = ( le1 <= 
					points[n][nn]*v[nn].get_period()-1);
				cp.add_constraint(cs);
			}
		}

		PPL::Variable xx(i);
		PPL::Linear_Expression le_i;
		le_i += k*xx + v[i].get_jitter();
		for (int j = 0; j < i; j++) {
			PPL::Variable xx(j);
			le_i += xx * points[n][j];
		}
		if( i == ntasks-1) {
			PPL::Constraint cs = (
				le_i <= k*v[i].get_dline());
			cp.add_constraint(cs);
		}
		for (unsigned m = i + 1; m < ntasks; m++) {
			PPL::Linear_Expression le_i1 = le_i;
			PPL::Variable xx(m);
			le_i1 += xx - 1;
			PPL::Constraint cs = (
				le_i1 <= k*v[i].get_dline());
			cp.add_constraint(cs);
		}
		
		ps.add_disjunct(cp);
	}

	ps_i.intersection_assign(ps);
}

static void
constraints_of_np_task_i(const std::vector<Scan::FPTask> &v,
				const PPL::C_Polyhedron &base,
				const int ntasks, int i,
				PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps_i)
{

	if( i == 0) {
		PPL::Pointset_Powerset<PPL::C_Polyhedron> 
						ps(ntasks, EMPTY);
		PPL::C_Polyhedron cp = base;
		PPL::Variable xx(i);
		PPL::Linear_Expression le_i;
		le_i += v[i].get_jitter() + xx;
		for (int j = i+1; j < ntasks; j++) {
			PPL::Linear_Expression le_ij= le_i;
			PPL::Variable xx(j);
			le_ij += xx -1;
			PPL::Constraint cs_i = (
				le_ij <= v[i].get_dline());
			cp.add_constraint(cs_i);
		}
		ps.add_disjunct(cp);
		ps_i.intersection_assign(ps);
		
		return;
	}

	/* The upper bound of the i_th task's blocking time.*/
	int B_i = compute_max_blocking_time(v.begin() + i + 1, v.end());
	
	/* The upper bound of length of the level-i active period. */
	int H_i = 1;
	for ( int j = 0; j <= i; j++)
		H_i =  boost::math::lcm(H_i, (int)(v[j].get_period()));
	H_i += B_i;

	/* The number of jobs to be considered. */
	int K_i = H_i/v[i].get_period();
	if( H_i%v[i].get_period())
		K_i ++;

	for( int k = 1; k <= K_i; k++)
		constraints_of_np_job_i_k(v, base, ntasks, i, k, ps_i);
};

static void
constraints_of_np_job_i_k2(const std::vector<Scan::FPTask> &v,
			const PPL::C_Polyhedron &base,
			const int ntasks, const int i, const int k,
			PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps_i,
			const int nvars,
			PPL::Pointset_Powerset<PPL::C_Polyhedron> &sys)
{
	vector< vector<int> > points;
	vector<int> pp = compute_all_points(v.begin(), v.end() - (ntasks - i),
						k*v[i].get_dline());
	points = number_of_instances(pp.begin(), pp.end(),
					v.begin(), v.end() - (ntasks - i));

	/* Constraints for job k of task i */
	PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(nvars, EMPTY);
	
	for(unsigned n = 0; n < points.size(); n++) {
		PPL::C_Polyhedron cp(nvars);// = base;

		for( unsigned nn = 0; nn < i; nn++) {
			PPL::Linear_Expression le;
			
			PPL::Variable xx(v[i].get_id());
			PPL::Variable xj(v[nn].get_id()+2);
			le += xx*(k-1) + xj;//v[nn].get_jitter();
			for( int j = 0; j < i; j++) {
				PPL::Variable xx(v[j].get_id());
				le += xx * points[n][j];
			}
			if( i == ntasks-1) {
				PPL::Constraint cs = ( le <= 
					points[n][nn]*v[nn].get_period()-1);

				cp.add_constraint(cs);
			}
			for (unsigned m = i + 1; m < ntasks; m++) {
				PPL::Linear_Expression le1 = le;
				PPL::Variable xx(v[m].get_id());
				le1 += xx - 1;
				PPL::Constraint cs = ( le1 <= 
					points[n][nn]*v[nn].get_period()-1);
				cp.add_constraint(cs);
			}
		}

		PPL::Variable xx(v[i].get_id());
		PPL::Variable xj(v[i].get_id()+2);
		PPL::Linear_Expression le_i;
		le_i += k*xx + xj;//v[i].get_jitter();
		for (int j = 0; j < i; j++) {
			PPL::Variable xx(v[j].get_id());
			le_i += xx * points[n][j];
		}
		if( i == ntasks-1) {
			PPL::Variable xd(v[i].get_id()+1);
			PPL::Constraint cs = (
				le_i <= k*xd);//v[i].get_dline());
			cp.add_constraint(cs);
		}
		for (unsigned m = i + 1; m < ntasks; m++) {
			PPL::Linear_Expression le_i1 = le_i;
			PPL::Variable xx(v[m].get_id());
			le_i1 += xx - 1;
			PPL::Variable xd(v[i].get_id()+1);
			PPL::Constraint cs = (
				le_i1 <= k*xd);//v[i].get_dline());
			cp.add_constraint(cs);
		}
		
		ps.add_disjunct(cp);
	}

	///ps_i.intersection_assign(ps);
	sys.intersection_assign(ps);
}

static void
constraints_of_np_task_i2(const std::vector<Scan::FPTask> &v,
				const PPL::C_Polyhedron &base,
				const int ntasks, int i,
				PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps_i,
				const int nvars,
				ConstraintsSystem & dis,
				vector<string> & vars_list)
				//PPL::Pointset_Powerset<PPL::C_Polyhedron> &sys)
{

PPL::Pointset_Powerset<PPL::C_Polyhedron> &sys = dis.poly;
	/* The upper bound of the i_th task's blocking time.*/
	//int B_i = compute_max_blocking_time(v.begin() + i + 1, v.end());
	int B_i = compute_max_blocking_time_pline(v.begin() + i, v.end());
	
	/* The upper bound of length of the level-i active period. */
	int H_i = 1;
	for ( int j = 0; j <= i; j++)
		H_i =  boost::math::lcm(H_i, (int)(v[j].get_period()));
	H_i += B_i;

	/* The number of jobs to be considered. */
	int K_i = H_i/v[i].get_period();
	if( H_i%v[i].get_period())
		K_i ++;
	for( int k = 1; k <= K_i; k++) {

	/**
	   each loop corresponds to a call of the funtion
		constraints_of_np_job_i_k2
	*/
	vector< vector<int> > points;
	vector<int> pp = compute_all_points(v.begin(), v.end() - (ntasks - i),
				(k-1)*v[i].get_period() + v[i].get_dline());

        sort(pp.begin(), pp.end());
        pp.erase( unique( pp.begin(), pp.end() ), pp.end());	    

	points = number_of_instances_pline(pp.begin(), pp.end(),
					v.begin(), v.end() - (ntasks - i));

	cout<<"Task "<<v[i].get_name()<<endl;
	cout<<"Instance k = "<<k<<endl;
	cout<<"Points: ";
	print_vector(pp.begin(), pp.end());
	cout<<endl;
	cout<<"Vector n: ";
	for(unsigned po=0; po<points.size(); po++) {
		print_vector(points[po].begin(), points[po].end());
		cout<<";";
	}
	cout<<endl;

	cout<<"Now preparing the pointset"<<endl;
	/* Constraints for job k of task i */
	PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(nvars, EMPTY);
	
	for(unsigned n = 0; n < points.size(); n++) {
		cout << "Point: ";
		print_vector(points[n].begin(), points[n].end());
		cout<<endl;

		PPL::C_Polyhedron cp = base;
		// The (i - 1) first inequalities in the formula
		for( unsigned nn = 0; nn < i; nn++) {
			if( points[n][nn] == 0 )
				continue;
			PPL::Linear_Expression le;
			
			PPL::Variable xx(3*i);
			PPL::Variable xj(3*nn+2);
			le += xx*(k-1) + xj;

			for( int j = 0; j < i; j++) {
				PPL::Variable xx(3*j);
				le += xx * points[n][j];
			}
			// There is no blocking time for the lowest priority task
			if( i == ntasks-1) {
				PPL::Constraint cs = ( le <= 
					points[n][nn]*v[nn].get_period()-1);

				cp.add_constraint(cs);
			}
			bool no_blocking = true;
			for (unsigned m = i + 1; m < ntasks; m++) {
				if( v[m].get_pipeline_tag() == v[i].get_pipeline_tag() && v[m].get_pipeline_tag() >= 0) {
					if( v[m].get_dline() <= v[m].get_period())
						continue;
				}
				PPL::Linear_Expression le1 = le;
				PPL::Variable xx(3*m);
				le1 += xx;// - 1;
				no_blocking = false;
				PPL::Constraint cs = ( le1 <= 
					points[n][nn]*v[nn].get_period()-1);
				cp.add_constraint(cs);
			}
			if( no_blocking) {
				PPL::Constraint cs = ( le <=
                                        points[n][nn]*v[nn].get_period()-1);
                                cp.add_constraint(cs);
			}

		}

		PPL::Variable xx(3*i);
		PPL::Variable xj(3*i+2);
		PPL::Linear_Expression le_i;
		le_i += k*xx + xj;
		for (int j = 0; j < i; j++) {
			PPL::Variable xx(3*j);
			le_i += xx * points[n][j];
		}
		if( i == ntasks-1) {
			PPL::Variable xd(3*i+1);
			PPL::Constraint cs;
			cs = (le_i - xd <= (k-1)*v[i].get_period());
			cp.add_constraint(cs);
		}
		bool no_blocking = true;
		for (unsigned m = i + 1; m < ntasks; m++) {
			if( v[m].get_pipeline_tag() == v[i].get_pipeline_tag() && v[m].get_pipeline_tag() >= 0) {
				if( v[m].get_dline() <= v[m].get_period())
					continue;
			}
			no_blocking = false;
			PPL::Linear_Expression le_i1 = le_i;
			PPL::Variable xx(3*m);
			le_i1 += xx;// - 1;
			PPL::Variable xd(3*i+1);
			PPL::Constraint cs;
			cs = ( le_i1 - xd <= (k-1)*v[i].get_period());
			cp.add_constraint(cs);
		}
		if( no_blocking) {
			PPL::Variable xd(3*i+1);
			PPL::Constraint cs;
			cs = (le_i - xd <= (k-1)*v[i].get_period());
			cp.add_constraint(cs);
		}
		//if( !check_containment(cp, ps))
		ps.add_disjunct(cp);
	} // the closed parenthesis of loop ''for(unsigned n = 0; n < points.size(); n++)''
          if( !ps.empty())
{
            // Here, we must insantiate variables not in the vars-list 
        PPL::Variables_Set vars__;
        for(int i = 0; i < dis.vars.size(); i++) { // it = sys.vars.begin(); it != sys.vars.end(); it++) {
                if( get_index(vars_list, dis.vars[i]) != -1) continue;
                vector<string> ss = split(dis.vars[i], ".");
                int ti = find_task(v, ss[0]);
                if( v[ti].get_pipeline_tag() == -1 || ss[1].compare("wcet")==0) {
                        double v_ = get_value_from_task(v[ti], ss[1]);

                        Variable xx(i);
                        Congruence cg = ((xx %= int(v_)) / 0);
                        ps.refine_with_congruence(cg);
                        vars__.insert(xx);
                }
//                else if( ss[1].compare("wcet") == 0) {
//                        double v_ = get_value_from_task(v[ti], ss[1]);
//
//                        Variable xx(i);
//                        Congruence cg = ((xx %= int(v_)) / 0);
//                        ps.refine_with_congruence(cg);
//                        vars__.insert(xx);
//                }
        }
        ps.remove_space_dimensions(vars__);
	//if( !sys.contains(ps))
            sys.intersection_assign(ps);
}


//	sys.intersection_assign(ps);
	} // the closed parenthesis of loop ''for( int k = 1; k <= K_i; k++) ''
};

void np_build_general_sensitivity(std::vector<Scan::FPTask> &v, ConstraintsSystem &dis, vector<string> & vars_list)
{
	int ntasks = v.size();
	int nvars = dis.vars.size(); 	

	/**
	 * the basic constraint on each wcet that 0 <= C_i <= min(T_{i}, D_{i})
	 **/
	PPL::C_Polyhedron base(nvars);
    	for (int i=0;i<ntasks;i++) {
        	PPL::Variable xw(3*i);//v[i].get_id());

        	PPL::Constraint cs_min = (xw >= 0);
        	base.add_constraint(cs_min);
        	dis.poly.add_constraint(cs_min);

        	PPL::Constraint cs_max = (xw <= (int)v[i].get_period());
        	base.add_constraint(cs_max);
        	dis.poly.add_constraint(cs_max);
		dis.vars[3*i] = v[i].get_name() + ".wcet";

        	PPL::Variable xd(3*i+1);//v[i].get_id()+1);
        	PPL::Constraint dd_min = (xd >= 0);
        	base.add_constraint(dd_min);
        	dis.poly.add_constraint(dd_min);
        	PPL::Constraint dd_max = (xd <= int(v[i].get_dline()));
        	base.add_constraint(dd_max);
        	dis.poly.add_constraint(dd_max);
		dis.vars[3*i+1] = v[i].get_name() + ".dline";

        	PPL::Variable xj(3*i+2);//v[i].get_id()+2);
        	PPL::Constraint j_min = (xj >= 0);
        	base.add_constraint(j_min);
        	dis.poly.add_constraint(j_min);
        	PPL::Constraint j_max = (xj + xw <= int(v[i].get_dline()));
		dis.vars[3*i+2] = v[i].get_name() + ".jitter";

        	base.add_constraint(j_max);
        	dis.poly.add_constraint(j_max);
        }
	PPL::Variables_Set vars_;
	vector<string> backup = dis.vars;
        for(int i = 0; i < dis.vars.size(); i++) { // it = sys.vars.begin(); it != sys.vars.end(); it++) {
                if( get_index(vars_list, dis.vars[i]) != -1) continue;
                vector<string> ss = split(dis.vars[i], ".");
                int ti = find_task(v, ss[0]);
                if( v[ti].get_pipeline_tag() == -1 || ss[1].compare("wcet")==0) {
                        double v_ = get_value_from_task(v[ti], ss[1]);

                        Variable xx(i);
                        Congruence cg = ((xx %= int(v_)) / 0);
                        dis.poly.refine_with_congruence(cg);
                        vars_.insert(xx);
			backup.erase(backup.begin() + get_index(backup, dis.vars[i]));

                }
//                else if( ss[1].compare("wcet") == 0) {
//                        double v_ = get_value_from_task(v[ti], ss[1]);
//
//                        Variable xx(i);
//                        Congruence cg = ((xx %= int(v_)) / 0);
//                        dis.poly.refine_with_congruence(cg);
//                        vars_.insert(xx);
//                }
        }
        dis.poly.remove_space_dimensions(vars_);
	//base.remove_space_dimensions(vars_);


        for( int i = 0; i < ntasks; i++) {
		/** 
		   ps_i has been deprecated, now its redundent...  
		 */			
		PPL::Pointset_Powerset<PPL::C_Polyhedron> ps_i(nvars, EMPTY);
		//constraints_of_np_task_i2(v, base, ntasks, i, ps_i, nvars, dis.poly);
		constraints_of_np_task_i2(v, base, ntasks, i, ps_i, nvars, dis, vars_list);

	}
	dis.vars = backup;
}

void build_general_sensitivity2(vector<FPTask> &v, ConstraintsSystem &node_cs)
{
    int ntasks = v.size();
    int nvars = node_cs.vars.size();
    ConstraintsSystem sys(nvars);

    PPL::C_Polyhedron base(nvars);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xw(v[i].get_id());

        PPL::Constraint cs_min = (xw >= 0);
        base.add_constraint(cs_min);
	node_cs.poly.add_constraint(cs_min);

        PPL::Constraint cs_max = (xw <= (int)v[i].get_period());
        base.add_constraint(cs_max);
	node_cs.poly.add_constraint(cs_max);

        PPL::Variable xd(v[i].get_id()+1);
        PPL::Constraint dd_min = (xd >= 0);
        base.add_constraint(dd_min);
	node_cs.poly.add_constraint(dd_min);
        PPL::Constraint dd_max = (xd <= int(v[i].get_dline()));
        base.add_constraint(dd_max);
	node_cs.poly.add_constraint(dd_max);

        PPL::Variable xj(v[i].get_id()+2);
        PPL::Constraint j_min = (xj >= 0);
        base.add_constraint(j_min);
	node_cs.poly.add_constraint(j_min);
        //PPL::Constraint j_max = (xj + xw <= int(v[i].get_dline()));
        PPL::Constraint j_max = (xj <= int(v[i].get_dline()));
        base.add_constraint(j_max);
	node_cs.poly.add_constraint(j_max);

    }

    for (int i=0; i<ntasks; i++) {
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
                number_of_instances_pline(pp.begin(), pp.end(), 
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
                        PPL::Variable xx(v[j].get_id()); 
                        le += xx * myn[n][j];
                    }
                    PPL::Variable xx(v[i].get_id()); 
                    PPL::Variable xj(v[k].get_id()+2);
                    le += h * xx;
                    le += xj;
                    PPL::Constraint cs = (le <= myn[n][k]*v[k].get_period());
                    cp.add_constraint(cs);
                }
                
                PPL::Linear_Expression le;
                for (unsigned j=0; j<i; j++) {
                    PPL::Variable xx(v[j].get_id()); 
                    le += xx * myn[n][j];
                }
                PPL::Variable xx(v[i].get_id());
                PPL::Variable xd(v[i].get_id()+1);
                PPL::Variable xj(v[i].get_id()+2);//3*i+2);
                le += h * xx;
                //le += -xd+xj;  // to be tested
		le += -xd+xj;
                PPL::Constraint cs = (le <= (h-1)*v[i].get_period());
                cp.add_constraint(cs);

                //cout << "Constraint set completed: " << cp << endl;
                cout << "Constraint set completed: " << endl;

                ps.add_disjunct(cp);
                ps.omega_reduce();
            }
            cout << "Now all constraint set have been prepared, intesecting...";
            cout.flush();
            //sys.poly.intersection_assign(ps);
            node_cs.poly.intersection_assign(ps);
            cout << "... completed!" << endl;
        }
    }
}

void constraints_of_a_pipepline(Scan::Pipeline &pline, 
			DisSysVisitor &vis, ConstraintsSystem &dis)
{
	int ntasks = pline.tasks.size();
	int nvars = dis.vars.size();
	ConstraintsSystem sys(nvars);
	vector<Scan::FPTask> &v = pline.tasks;

	/** This is really dirty ... for sensitivity analysis */
	if( ntasks == 1) {
		int i = find_task(vis.v, v[0].get_name());
		vis.v[i].set_pipeline_pos(0);
		return;
	}
	for( int i = 0; i < ntasks; i++) {
		//int index = find_task(vis.v, v[i].get_name());
		string jitter = ".jitter";
		string i_jitter = v[i].get_name() + jitter;
		int index = get_index(dis.vars, i_jitter); 
		if( index != -1) {
			PPL::Variable xj(index);
		if( i == 0) {
//			PPL::Constraint cs_jitter = (xj==0);
//			dis.poly.add_constraint(cs_jitter);
		}
		else {
			//int pre_index = find_task(vis.v, v[i-1].get_name());
			string dline = ".dline";
			string j_dline = v[i-1].get_name() + dline;
			int pre_index = get_index(dis.vars, j_dline); 
			PPL::Variable xd(pre_index);
			PPL::Constraint cs_jitter = (xd-xj<=0);//(xd <= xj);
			dis.poly.add_constraint(cs_jitter);
		} }
	}
}	

void remove_jitter_dline(ConstraintsSystem & sys, vector<Scan::FPTask> &v, DisSysVisitor &vis);
void merge_pline_constraints(ConstraintsSystem & sys, vector<Scan::FPTask> &pline_tasks, DisSysVisitor &vis, vector<string> &vars_list);
ConstraintsSystem dis_build_hyperplanes_powerset(DisSysVisitor &vis, vector<string> &vars_list)
{
        for( int i = 0; i < vis.pipelines.size(); i++ ) {
                Scan::FPTask task;
                if( vis.pipelines[i].tasks.size() == 1) {
                        vis.pipelines[i].tasks[0].set_pipeline_tag(-1);
                        task = vis.pipelines[i].tasks[0];
                }
                else
                        continue;

                for( std::vector<Scan::FPTask>::iterator it = vis.v.begin(); it != vis.v.end(); ++it)
                        if( task.get_name().compare(it->get_name()) == 0)
                                it->set_pipeline_tag(-1);
                for( int j = 0; j < vis.MAX_; j++ ) {
                        for( std::vector<Scan::FPTask>::iterator it = vis.node[j].v_fpfp.begin(); it != vis.node[j].v_fpfp.end(); it++)
                                if( task.get_name().compare(it->get_name()) == 0)
                                        it->set_pipeline_tag(-1);
                        for( std::vector<Scan::FPTask>::iterator it = vis.node[j].v_fpnp.begin(); it != vis.node[j].v_fpnp.end(); it++)
                                if( task.get_name().compare(it->get_name()) == 0)
                                        it->set_pipeline_tag(-1);
                        for( std::vector<Scan::FPTask>::iterator it = vis.node[j].v_tasks.begin(); it != vis.node[j].v_tasks.end(); it++)
                                if( task.get_name().compare(it->get_name()) == 0)
                                        it->set_pipeline_tag(-1);
                }
        }


	/**
	 *  For each computational resource node in the distributed system,
	 *  we will first compute its local constraints.
	 **/
	vector<ConstraintsSystem*> nodes;
	for( int i = 0; i < vis.MAX_; i++) {
		int nvars_ = vis.node[i].get_nvars();
		ConstraintsSystem *p_cs = new ConstraintsSystem(nvars_);
		PPL::C_Polyhedron true_(nvars_);
		/* nodes[i] contains local constraints for the i-th node. */
		nodes.push_back(p_cs);
		/* Each local constraints are initialized to be true. */
		nodes[i]->poly.add_disjunct(true_);
	}
		
	/* Here constraints on each node are built. */
	for (int i = 0; i < vis.MAX_; i++) {
		if( vis.node[i].get_nvars())
			cout<<endl<<endl<<"Building constrains on Node "
							<<i+1<<endl<<endl;

		if( vis.node[i].v_fpfp.size())
			*nodes[i] = build_general_sensitivity_pline(vis.node[i].v_fpfp, vars_list);
		else if(vis.node[i].v_fpnp.size())
			np_build_general_sensitivity(vis.node[i].v_fpnp, *nodes[i], vars_list);

		remove_jitter_dline(*nodes[i], vis.node[i].v_tasks, vis);
	}

	cout<<"Now, to merge constraints on different nodes :\n";
	vector<string> vars_ = nodes[0]->vars;
	nodes[0]->vars.clear();
	for( int j = 0; j < vars_.size(); j++) {
		vector<string> ss = split(vars_[j], ".");
		int ti = find_task(vis.node[0].v_tasks, ss[0]);
		if ( get_index(vars_list, vars_[j]) != -1  ||  (vis.node[0].v_tasks[ti].get_pipeline_tag() != -1 && ss[1].compare("wcet") != 0) )
			nodes[0]->vars.push_back(vars_[j]);
	}
	vector<Scan::FPTask> pline_tasks;
	for( vector<Scan::FPTask>::iterator it = vis.node[0].v_tasks.begin(); it != vis.node[0].v_tasks.end(); it++)
                if( it->get_pipeline_tag() != -1)
                        pline_tasks.push_back(*it);

	for( int i = 1; i < vis.MAX_; i++) {
		if( vis.node[i].v_tasks.size() == 0 ) continue;
		//to merge constraints on i-th node
		nodes[0]->poly.concatenate_assign(nodes[i]->poly);
		//to update the variable names (for people to see)
		for( int j = 0; j < nodes[i]->vars.size(); j++) {
			vector<string> ss = split(nodes[i]->vars[j], ".");
			int ti = find_task(vis.node[i].v_tasks, ss[0]);
			if ( get_index(vars_list, nodes[i]->vars[j]) != -1 || (vis.node[i].v_tasks[ti].get_pipeline_tag() != -1 && ss[1].compare("wcet") != 0 ))
				nodes[0]->vars.push_back(nodes[i]->vars[j]);
		}
		for( vector<Scan::FPTask>::iterator it = vis.node[i].v_tasks.begin(); it != vis.node[i].v_tasks.end(); it++)
			if( it->get_pipeline_tag() != -1)
				pline_tasks.push_back(*it);

		merge_pline_constraints(*nodes[0], pline_tasks, vis, vars_list);
		cout<<"Constraints on node "<<i+1<<" has been merged"<<endl<<endl;
	}
	/**
	   The tasks order in this vector may not correspond to the 
	   variable name at the same position in vector nodes[0]->vars. 
           So we re-order its tasks here.
	 */	
	vis.v.clear();
	for( int i = 0; i < vis.MAX_; i++) {
		if( vis.node[i].v_tasks.size() == 0 ) continue;
		for( int j = 0; j < vis.node[i].v_tasks.size(); j++)
			vis.v.push_back(vis.node[i].v_tasks[j]);
	}
		
	cout<<endl;
	cout<<endl;
	/* To add constraints of each pipeline to system constraints. */
	for(vector<Scan::Pipeline>::iterator it=vis.pipelines.begin();
				it != vis.pipelines.end(); ++it) 
//		constraints_of_a_pipepline(*it, vis, *nodes[0]);

	return *nodes[0];
}

void merge_pline_constraints(ConstraintsSystem & sys, vector<Scan::FPTask> &pline_tasks, DisSysVisitor &vis, vector<string> &vars_list)
{
	vector<Scan::FPTask> tasks = pline_tasks;
	for( vector<Scan::FPTask>::iterator it = tasks.begin(); it != tasks.end(); it++) {
		string dline = it->get_name() + ".dline";
		int pos = get_index(sys.vars, dline);
		if( pos == -1) continue;
		for(vector<Scan::Pipeline> :: iterator jt = vis.pipelines.begin(); jt != vis.pipelines.end(); jt++) {
			bool flag = false;
			vector<Scan::FPTask> &vect = jt->tasks;
			for ( int i = 0; i < vect.size(); i++) {
                                        if(vect[i].get_name().compare(it->get_name()) != 0) continue;
					 if( i == vect.size() - 1) {
						 if( get_index(vars_list, dline) == -1) {
                                                        PPL::Variable xd(pos);
                                                        PPL::Variables_Set vars_set;
                                                        vars_set.insert(xd);
                                                        sys.poly.remove_space_dimensions(vars_set);
                                                        sys.vars.erase(sys.vars.begin()+pos);
						}
                                        }
					else {
						Scan::FPTask &task = vect[i+1];
						string jitter = task.get_name() + ".jitter";
						int j = get_index(sys.vars, jitter);
						if( j == -1) { flag = true; break;} 
						PPL::Variables_Set set;
						PPL::Variable xd(pos);
						PPL::Variable xj(j);
						set.insert(xd);
						set.insert(xj);
						PPL::Constraint cs_jitter = (xd-xj<=0);//(xd <= xj);
                        			sys.poly.add_constraint(cs_jitter);
						sys.poly.remove_space_dimensions(set);
						sys.vars.erase(sys.vars.begin() + get_index(sys.vars, dline));
						sys.vars.erase(sys.vars.begin() + get_index(sys.vars, jitter));
					}
					flag = true;
					break;
			}
			if( flag) break;
		}
	}
}


void remove_jitter_dline(ConstraintsSystem & sys, vector<Scan::FPTask> &v, DisSysVisitor &vis)
{
	for( vector<Scan::FPTask>::iterator it = v.begin(); it != v.end(); it++) {
		if( it->get_pipeline_tag() == -1) continue;
		if( it->get_pipeline_pos() != 1) continue;
		string jitter = it->get_name() + ".jitter";
		int pos = get_index(sys.vars, jitter);
		if( pos == -1) throw ("Variable not found");
		PPL::Variable xj(pos);
                PPL::Constraint cs_jitter = (xj==0);
                sys.poly.add_constraint(cs_jitter);
		PPL::Variables_Set vars_set;
		vars_set.insert(xj);
		sys.poly.remove_space_dimensions(vars_set);
		sys.vars.erase(sys.vars.begin()+pos);
	}
//
//	for( vector<Scan::FPTask>::iterator it = v.begin(); it != v.end(); it++) {
//		if( it->get_pipeline_tag() == -1) continue;
//		string dline = it->get_name() + ".dline";
//                int pos = get_index(sys.vars, dline);
//		if( pos == -1) continue;
//		for(vector<Scan::Pipeline> :: iterator jt = vis.pipelines.begin(); jt != vis.pipelines.end(); jt++) {
//			bool flag = false;
//			vector<Scan::FPTask> &vect = jt->tasks;
//                        for ( int i = 0; i < vect.size(); i++) {
//				if(vect[i].get_name().compare(it->get_name()) != 0) continue;
//				if( i == vect.size() - 1) {
//					PPL::Variable xd(pos);
//					PPL::Variables_Set vars_set;
//					vars_set.insert(xd);
//					sys.poly.remove_space_dimensions(vars_set);
//					sys.vars.erase(sys.vars.begin()+pos);
//				}
//				flag = true;
//				break;
//			}
//			if (flag) break;
//		}
//	}	
}

void ConstraintsSystem::print_points(string fname) {
	int index = -1;
	for( PPL::Pointset_Powerset<PPL::C_Polyhedron>::iterator 
				i = poly.begin(); i != poly.end(); i++) {
		index ++;
		PPL::C_Polyhedron cp = i->pointset();
		PPL::Generator_System gs = cp.generators();
		
		ofstream res;
		string index_str = static_cast<ostringstream*>( 
					&(ostringstream() << index) )->str();
		string out = fname; //to_string(index));
		out.append("_");
		out.append(index_str);
		out.append(".out");
		res.open(out.c_str());

		for( PPL::Generator_System::const_iterator
				 it = gs.begin(); it != gs.end(); it++) {
			if( !it->is_point()) throw("Not a point here");
			for( PPL::dimension_type ii = it->space_dimension(); 
								ii -- > 0;) {
				stringstream ss1 (stringstream::in | stringstream::out);
				stringstream ss2 (stringstream::in | stringstream::out);
				ss1 << it->coefficient(Variable(ii));
				ss2 << it->divisor();
				double coe, div;
				ss1 >> coe;
				ss2 >> div;
				double x = coe/div;
				res<<x<<"	";	
			}
			res<<"\n";
		}
	}
}

		



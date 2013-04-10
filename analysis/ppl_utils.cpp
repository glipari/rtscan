#include <analysis/ppl_utils.hpp>

#include <analysis/hyperplane.hpp>
#include <analysis/task_utility.hpp>

#include <common/string_utils.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

namespace Scan {

void SysVisitor::operator()(const Scan::Property &p) {
	if( p.name == "Period")
		pipelines.back().set_period(std::atoi(p.value.c_str()));
	else if( p.name == "E2Edline")
		pipelines.back().set_e2e_dline(std::atoi(p.value.c_str()));
	else 
        std::cerr << "ERROR: parsing a property at the sys level\n"; 
}

void SysVisitor::operator()(const Scan::PropertyList &pl) {

	if (pl.type == "sys") 
            for ( auto &x : pl.children) 
				boost::apply_visitor(*this, x);
	else if (pl.type == "pipeline") {
	    	pipelines.push_back(Scan::create_pipeline(pl));
            for ( auto &x : pl.children) 
				boost::apply_visitor(*this, x);
	}
	else if (pl.type == "task") { 
		Scan::FPTask_ptr t = Scan::create_fp_task_ptr(pl);

		if( t->get_pipeline_pos() >= 0) 
			pipelines.back().register_a_fp_task(t);

		node[t->get_node()-1].register_a_fp_task(t);
		all_tasks.push_back(t);

		v.push_back(*t);
	}
	else 
		throw runtime_error("Found a property list which is not a task!");
}

void SysVisitor::operator() (const std::string &vars_list_file)
{
	if( vars_list_file !="" ) {
		std::ifstream vars_stream(vars_list_file.c_str());
		while (!vars_stream.eof()) {
			string line;
			getline(vars_stream, line);
			line = StrUtils::remove_spaces(line);
			if (line != "") vars_list.push_back(line);
		}
		return;
	}

	for ( int i = 0; i < all_tasks.size(); i++) {
		vars_list.push_back( all_tasks[i]->get_name() + ".wcet");
		vars_list.push_back( all_tasks[i]->get_name() + ".dline");
		vars_list.push_back( all_tasks[i]->get_name() + ".jitter");
	}
}

template<class IterInt, class IterTask> vector< vector<int> > 
number_of_instances(IterInt pb, IterInt pe, IterTask tb, IterTask te) 
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

bool is_a_pline_task(const Scan::FPTask & task) {
	return task.get_pipeline_pos() >= 0;
}

bool is_a_pline_task(const Scan::FPTask_ptr task) {
	return task->get_pipeline_pos() >= 0;
}


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

/** find the task with a certain name */
int find_task(const std::vector<Scan::FPTask_ptr> &tset, const std::string &name)
{
    for (unsigned i=0; i<tset.size(); i++) 
        if (tset[i]->get_name() == name) return i;
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


ConstraintsSystem::ConstraintsSystem() {}

ConstraintsSystem::ConstraintsSystem(int n, PPL::Degenerate_Element kind) :
	poly(n, kind), vars(n)
{}

struct com_pair_class {
  bool operator() (pair<double, double> p1, pair<double, double> p2) { 
		if( p1.first == 0 && p1.second == 0) return true;
		if( p2.first == 0 && p2.second == 0) return false;
		if( p1.second < p2.second) return true;
	        if( p1.second == p2.second ) return (p1.first > p2.first);
        	return false;
   }
} com_pair;
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
		vector< pair<double, double> > points;
		for( PPL::Generator_System::const_iterator
				 it = gs.begin(); it != gs.end(); it++) {
			if( !it->is_point()) throw("Not a point here");
			pair<double, double> point; 
			for( PPL::dimension_type ii = it->space_dimension(); 
								ii -- > 0;) {
				stringstream ss1 (stringstream::in | stringstream::out);
				stringstream ss2 (stringstream::in | stringstream::out);
				ss1 << it->coefficient(Variable(ii));
				ss2 << it->divisor();
				double coe, div;
				ss1 >> coe;
				ss2 >> div;
				if( ii != 0) point.first = coe/div;
				else point.second = coe/div;
			}
			points.push_back(point);
		}
		std::sort(points.begin(), points.end(), com_pair);
		for( unsigned j = 0; j < points.size(); j++) 
			res<<points[j].first<<"	"<<points[j].second<<"\n";
	}
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
                    const std::vector<FPTask> &tasks,
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

void ConstraintsSystem::do_sensitivity(
                    const std::vector<Scan::FPTask_ptr> &v,
                    const std::string &var)
{
    vector<FPTask> tasks;
    for( auto e : v)
	    tasks.push_back(*e);

    PPL::Pointset_Powerset<PPL::C_Polyhedron> copied(poly);

    int k = get_index(vars, var);   // we do sensitivity on the k variable
    if (k == -1) throw("Variable not found");

    for (unsigned i=0; i<vars.size(); i++)  {
        if (i == (unsigned)k) continue;

        vector<string> ss = split(vars[i], ".");
        int ti = find_task(tasks, ss[0]);
        if (ti == -1) throw "Task not found!";
	    if( tasks[ti].get_pipeline_pos() == -1 || ss[1]=="wcet") {
		    double v = get_value_from_task(tasks[ti], ss[1]);

		    Variable xx(i);
		    Congruence cg = ((xx %= int(v)) / 0); 
		    copied.refine_with_congruence(cg);
	    }
    }
	
    PPL::Variables_Set vars_;
    for (unsigned i=0; i<vars.size(); i++)  {
        if (i == (unsigned)k) continue;
	    Variable xx(i);
	    vars_.insert(xx);
    }
    copied.remove_space_dimensions(vars_);

    cout<< var << " : ";
    using namespace PPL::IO_Operators;
    IO_Operators::operator<<(cout, copied);
    cout<<"\n";
}


ConstraintsSystem SensitivityBuilder::build_hyperplanes_powerset() 
{

	/**
	   For each computational resource node in the distributed system,
	   we will first compute its local constraints.
	*/
	vector<boost::shared_ptr<ConstraintsSystem>> nodes;
	for( unsigned i = 0; i < vis->MAX_; i++) {
		boost::shared_ptr<ConstraintsSystem> ni( new ConstraintsSystem() );
		nodes.push_back(ni);
    }
		
	/* Here constraints on each node are built. */
	for (unsigned i = 0; i < vis->MAX_; i++) {
		if( vis->node[i].get_rq().size() == 0 ) continue;
		cout << "\nTo build constraints on node " << i + 1 << " ...\n";
		*nodes[i] = build_hyperplanes_powerset(i);

        max_bytes += nodes[i]->poly.total_memory_in_bytes();
	//	cout << "\nThe resulted local constraints' size ";
      //  cout << "(on node " << i + 1 << ") in bytes : ";
      //  cout << nodes[i]->poly.total_memory_in_bytes() <<endl;
//PPL::IO_Operators::operator << (std::cout, nodes[i]->poly);// 
//cout << endl;
//for (unsigned char ii=0; ii<nodes[i]->vars.size(); ii++) {
//    char c = 'A' + ii;
//    cout << c << ": " << nodes[i]->vars[ii] << endl;
//} 
	}
    curr_bytes = max_bytes;	
	cout<<"\nNow, to merge constraints on different nodes ...\n";

	vector<Scan::FPTask> pline_tasks;
	vector<Scan::FPTask_ptr> rq = vis->node[0].get_rq();
	for( auto t : rq)
        if( is_a_pline_task(t))
            pline_tasks.push_back(*t);

	for( unsigned i = 1; i < vis->MAX_; i++) {
		if( vis->node[i].get_rq().size() == 0 ) continue;
		/** To merge constraints on i-th node. */
        int pre_bytes = nodes[0]->poly.total_memory_in_bytes();
		nodes[0]->poly.concatenate_assign(nodes[i]->poly);
        int after_bytes = nodes[0]->poly.total_memory_in_bytes();
        curr_bytes += after_bytes - pre_bytes;
        if( curr_bytes > max_bytes) max_bytes = curr_bytes;
             
       
		/** To update the variable names. */
		for( unsigned j = 0; j < nodes[i]->vars.size(); j++) {
				nodes[0]->vars.push_back(nodes[i]->vars[j]);
		}

        /** Meanwhile, pipeline constraints are going to be removed. */ 
		vector<Scan::FPTask_ptr> rq = vis->node[i].get_rq();
		for( auto t : rq)
			if( is_a_pline_task(t))
				pline_tasks.push_back(*t);
        int released_bytes = nodes[i]->poly.total_memory_in_bytes();
        nodes[i].reset();
        curr_bytes -= released_bytes;

		merge_pline_constraints(pline_tasks, *nodes[0]);

		cout << "Constraints on node " << i + 1 << " has been merged.\n ";
//        cout << "the resulted Pointset_Powerset's size in bytes is ";
//        cout << nodes[0]->poly.total_memory_in_bytes() << endl;
	}

	cout << "\nThe parametric space for the whole system has been built!!!\n\n";
    cout << "The maximum memory used for polyhedra space in bytes : " << max_bytes << endl; 
    //cout << "The final memory used in bytes : " << nodes[0]->poly.total_memory_in_bytes() << endl; 
	return *nodes[0];
}

ConstraintsSystem SensitivityBuilder::build_hyperplanes_powerset(int index ) { 

	const ComputationalResource &node = vis->node[index];
	int fp = node.get_rq_fp().size();
	int npfp = node.get_rq_npfp().size();
	if( fp > 0 ) return this->build_general_sensitivity(index);
	else if ( npfp > 0 ) return this->build_general_sensitivity_np(index);
	//else if ( npfp > 0 ) return this->build_general_sensitivity_np_fast(index);
} 

PPL::C_Polyhedron SensitivityBuilder::build_base_constraints(int index)
{
     vector<FPTask_ptr> v = vis->node[index].get_rq();
	int ntasks = v.size();
	int nvars = 3*ntasks; 	
	PPL::C_Polyhedron base(nvars);
    for (int i=0;i<ntasks;i++) {
        PPL::Variable xw(3*i);

        PPL::Constraint cs_min = (xw >= 0);
        base.add_constraint(cs_min);
        PPL::Constraint cs_max = (xw <= (int)v[i]->get_period());
        base.add_constraint(cs_max);

        PPL::Variable xd(3*i+1);
        PPL::Constraint dd_min = (xd >= 0);
        base.add_constraint(dd_min);
        PPL::Constraint dd_max = (xd <= int(v[i]->get_dline()));
        base.add_constraint(dd_max);

        PPL::Variable xj(3*i+2);
        PPL::Constraint j_min = (xj >= 0);
        base.add_constraint(j_min);
        PPL::Constraint j_max = (xj + xw <= int(v[i]->get_dline()));
        base.add_constraint(j_max);
    }
    return base;
}   

ConstraintsSystem SensitivityBuilder::build_general_sensitivity(int index)
{
	vector<FPTask> v;
	for( auto x : vis->node[index].get_rq())
		v.push_back(*x);
    unsigned ntasks = v.size();
    unsigned nvars = ntasks * 3;
	PPL::C_Polyhedron base = build_base_constraints(index);
    ConstraintsSystem sys(nvars);
    for (unsigned i=0;i<ntasks;i++) {
        sys.vars[3*i] = v[i].get_name() + ".wcet";
        sys.vars[3*i+1] = v[i].get_name() + ".dline";
        sys.vars[3*i+2] = v[i].get_name() + ".jitter";
    }
    sys.poly.add_disjunct(base);
    remove_space_dimensions(index, sys.vars, sys.poly); 

    for (unsigned i=0; i<ntasks; i++) {
        // compute hyperperiod
        int hyper = compute_hyperperiod(v.begin(), v.end(), i+1);
        // now, for every h up to H/T_i
        unsigned h_max = 1;
        if (v[i].get_dline() > v[i].get_period()) 
            h_max = hyper / v[i].get_period();
        for (unsigned h = 1; h<=h_max; ++h) {
            // now we have to compute all vectors of n
            vector<int> pp = compute_all_points(v.begin(), v.end() - (ntasks-i),
                                    (h-1)*v[i].get_period() + v[i].get_dline());
            // To remove the redundant points.
            sort(pp.begin(), pp.end());
            pp.erase( unique( pp.begin(), pp.end() ), pp.end());	    

            vector< vector<int> > myn = 
                this->number_of_instances(pp.begin(), pp.end(), 
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
                    for (unsigned j=0; j<i; j++) {
                        PPL::Variable xx(3*j); 
                        le += xx * myn[n][j];
                    }
                    PPL::Variable xx(3*i); 
                    PPL::Variable xj(3*k+2);
                    le += h * xx;
                    le += xj;
                    if(myn[n][k] != 0) {
                        PPL::Constraint cs = 
                                    (le <= myn[n][k]*v[k].get_period());
		    	        cp.add_constraint(cs);	
		            }
                }
                
                PPL::Linear_Expression le;
                for (unsigned j=0; j<i; j++) {
                    if(myn[n][j] != 0) {
                        PPL::Variable xx(3*j); 
                        le += xx * myn[n][j];
                    }
                }
                PPL::Variable xx(3*i);
                PPL::Variable xd(3*i+1);
                PPL::Variable xj(3*i+2);
                le += h * xx;
                le += -xd+xj;  // to be tested
                PPL::Constraint cs = (le <= (h-1)*v[i].get_period());
                cp.add_constraint(cs);

                cout << "Constraint set completed: " << endl;
                ps.add_disjunct(cp);
            }
            cout << "Now all constraint set have been prepared, intesecting...";
            cout.flush();
            if( !ps.empty())
            {
                remove_space_dimensions(index, sys.vars, ps);
                sys.poly.intersection_assign(ps);
            } 
            cout << "... completed!" << endl;
        }
    }
    sys.vars=update_var_names(index, sys.vars);
    return sys;
}

ConstraintsSystem SensitivityBuilder::build_general_sensitivity_np(int index)
{
    vector<FPTask> v;
    for( auto x : vis->node[index].get_rq())
       v.push_back(*x);

	unsigned ntasks = v.size();
	unsigned nvars = 3*ntasks; 	
	PPL::C_Polyhedron base = build_base_constraints(index);
	ConstraintsSystem sys(nvars);
    for (unsigned i=0;i<ntasks;i++) {
        sys.vars[3*i] = v[i].get_name() + ".wcet";
        sys.vars[3*i+1] = v[i].get_name() + ".dline";
        sys.vars[3*i+2] = v[i].get_name() + ".jitter";
    }
    sys.poly.add_disjunct(base);
	remove_space_dimensions(index, sys.vars, sys.poly);

    int H_i = compute_hyperperiod(v.begin(), v.end(), ntasks);
    cout << "The hyperperiod is " << H_i << endl << endl;
    for( unsigned i = 0; i < ntasks; i++) {
		//constraints_of_np_task_i2(v, base, ntasks, i, nvars, sys, vars_list);
//        unsigned B_i = compute_max_blocking_time(v.begin() + i, v.end());
//        // The upper bound of length of the level-i active period.
//        unsigned H_i = 1;
//        for ( unsigned j = 0; j <= i; j++)
//        H_i =  boost::math::lcm(H_i, (unsigned)(v[j].get_period()));
//            H_i += B_i;
        /* The number of jobs to be considered. */
        unsigned K_i = H_i/v[i].get_period();
        if( H_i%v[i].get_period())
            K_i ++;
        for( unsigned k = 1; k <= K_i; k++) {
            vector< vector<int> > points;
            vector<int> pp = 
                        compute_all_points(v.begin(), v.end() - (ntasks - i),
                                    (k-1)*v[i].get_period() + v[i].get_dline());
            sort(pp.begin(), pp.end());
            pp.erase( unique( pp.begin(), pp.end() ), pp.end());
            points = this->number_of_instances(pp.begin(), pp.end(),
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

            PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(nvars, EMPTY);
            for(unsigned n = 0; n < points.size(); n++) {
                cout << "Point: ";
                print_vector(points[n].begin(), points[n].end());
                cout<<endl;

                PPL::C_Polyhedron cp = base;
                for( unsigned nn = 0; nn < i; nn++) {
                    if( points[n][nn] == 0 )
                        continue;
                    PPL::Linear_Expression le;
                    PPL::Variable xx(3*i);
                    PPL::Variable xj(3*nn+2);
                    le += xx*(k-1) + xj;
                    for( unsigned j = 0; j < i; j++) {
                         PPL::Variable xx(3*j);
                         le += xx * points[n][j];
                    }
                    // the (ntasks-1)-th (lowest prio) task has no blocking time
			        if( i == ntasks-1) {
				        PPL::Constraint cs = ( le <= 
					            points[n][nn]*v[nn].get_period()-1);
				        cp.add_constraint(cs);
			        }
                    else {
			            bool no_blocking = true;
			            for (unsigned m = i + 1; m < ntasks; m++) {
                            int tag_m = v[m].get_pipeline_tag();
                            int tag_i = v[i].get_pipeline_tag();
                            if( tag_m == tag_i && is_a_pline_task(v[m])) {
                                if( v[m].get_dline() <= v[m].get_period())
                                    continue;
				            }
				            PPL::Linear_Expression le1 = le;
				            PPL::Variable xx(3*m);
				            le1 += xx;
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
                }

		        PPL::Variable xx(3*i);
		        PPL::Variable xj(3*i+2);
		        PPL::Linear_Expression le_i;
		        le_i += k*xx + xj;
		        for (unsigned j = 0; j < i; j++) {
			        PPL::Variable xx(3*j);
			        le_i += xx * points[n][j];
		        }
		        if( i == ntasks-1) {
			        PPL::Variable xd(3*i+1);
			        PPL::Constraint cs;
			        cs = (le_i - xd <= (k-1)*v[i].get_period());
			        cp.add_constraint(cs);
		        }
                else {
		            bool no_blocking = true;
		            for (unsigned m = i + 1; m < ntasks; m++) {
                        int tag_m = v[m].get_pipeline_tag();
                        int tag_i = v[i].get_pipeline_tag();
			            if( tag_m == tag_i && is_a_pline_task(v[m])) {
				            if( v[m].get_dline() <= v[m].get_period())
					        continue;
			            }
			            no_blocking = false;
			            PPL::Linear_Expression le_i1 = le_i;
			            PPL::Variable xx(3*m);
			            le_i1 += xx;
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
                }                    

		        ps.add_disjunct(cp);
            }
            cout << "Now all constraint set have been prepared, intesecting...";
            if( !ps.empty()) {
                remove_space_dimensions(index, sys.vars, ps);
                sys.poly.intersection_assign(ps);
            }                                
            cout << "... completed!" << endl;
        }
	}
	sys.vars = update_var_names(index, sys.vars);
    return sys;
}

ConstraintsSystem SensitivityBuilder::build_general_sensitivity_np_fast(int index)
{
    vector<FPTask> v;
    for( auto x : vis->node[index].get_rq())
       v.push_back(*x);

	unsigned ntasks = v.size();
	unsigned nvars = 2*ntasks; 	
	PPL::C_Polyhedron base(nvars);// = build_base_constraints(index);
    for (int i=0;i<ntasks;i++) {
        //PPL::Variable xw(3*i);
        double xw = v[i].get_wcet();
//        PPL::Constraint cs_min = (xw >= 0);
//        base.add_constraint(cs_min);
//        PPL::Constraint cs_max = (xw <= (int)v[i]->get_period());
//        base.add_constraint(cs_max);

        PPL::Variable xd(2*i);
        PPL::Constraint dd_min = (xd >= 0);
        base.add_constraint(dd_min);
        PPL::Constraint dd_max = (xd <= int(v[i].get_dline()));
        base.add_constraint(dd_max);

        PPL::Variable xj(2*i+1);
        PPL::Constraint j_min = (xj >= 0);
        base.add_constraint(j_min);
        PPL::Constraint j_max = (xj + xw <= int(v[i].get_dline()));
        base.add_constraint(j_max);
    }
	ConstraintsSystem sys(nvars);
    for (unsigned i=0;i<ntasks;i++) {
        sys.vars[2*i] = v[i].get_name() + ".dline";
        sys.vars[2*i+1] = v[i].get_name() + ".jitter";
    }
    sys.poly.add_disjunct(base);


    for( unsigned i = 0; i < ntasks; i++) {
		int B_i = 0, bi=i, H_i;
		double U_i = 0, h_i=0;
		while( ++bi < ntasks) {
            int tag_bi = v[bi].get_pipeline_tag();
            int tag_i = v[i].get_pipeline_tag();
            if( tag_bi == tag_i && is_a_pline_task(v[bi])) {
                if( v[bi].get_dline() <= v[bi].get_period())
                    continue;
			}
			if (B_i < v[bi].get_wcet()) 
                B_i = v[bi].get_wcet();
        }
		for( bi = 0; bi <= i; bi++)	
			U_i += v[bi].get_wcet()/v[bi].get_period(); 
		for( bi = 0; bi <= i; bi++)
			h_i += v[bi].get_dline()*v[bi].get_wcet()/v[bi].get_period()
								+ v[bi].get_wcet();
		H_i = (B_i + h_i)/(1-U_i);
		H_i += 1;
		cout<< "H_i is " << H_i << endl;
        /* The number of jobs to be considered. */
        unsigned K_i = H_i/v[i].get_period();
        if( H_i%v[i].get_period())
            K_i ++;
        for( unsigned k = 1; k <= K_i; k++) {
            vector< vector<int> > points;
            vector<int> pp = 
                        compute_all_points(v.begin(), v.end() - (ntasks - i),
                                    (k-1)*v[i].get_period() + v[i].get_dline());
            sort(pp.begin(), pp.end());
            pp.erase( unique( pp.begin(), pp.end() ), pp.end());
            points = this->number_of_instances(pp.begin(), pp.end(),
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

            PPL::Pointset_Powerset<PPL::C_Polyhedron> ps(nvars, EMPTY);
            for(unsigned n = 0; n < points.size(); n++) {
                cout << "Point: ";
                print_vector(points[n].begin(), points[n].end());
                cout<<endl;

                PPL::C_Polyhedron cp = base;
                for( unsigned nn = 0; nn < i; nn++) {
                    if( points[n][nn] == 0 )
                        continue;
                    PPL::Linear_Expression le;
                    //PPL::Variable xx(3*i);
					double xx = v[nn].get_wcet();
                    PPL::Variable xj(2*nn+1);
                    le += xx*(k-1) + xj;
                    for( unsigned j = 0; j < i; j++) {
                         //PPL::Variable xx(3*j);
						 double xx = v[j].get_wcet();
                         le += xx * points[n][j];
                    }
                    PPL::Constraint cs = ( le + B_i <= 
                            points[n][nn]*v[nn].get_period()-1);
                    cp.add_constraint(cs);
                }

		//        PPL::Variable xx(3*i);
				double xx = v[i].get_wcet();
		        PPL::Variable xj(2*i+1);
		        PPL::Linear_Expression le_i;
		        le_i += k*xx + xj;
		        for (unsigned j = 0; j < i; j++) {
			        //PPL::Variable xx(3*j);
					double xx = v[j].get_wcet();
			        le_i += xx * points[n][j];
		        }
//		        if( i == ntasks-1) {
                PPL::Variable xd(2*i);
                PPL::Constraint cs;
                cs = (B_i + le_i - xd <= (k-1)*v[i].get_period());
                cp.add_constraint(cs);

		        ps.add_disjunct(cp);
            }
            cout << "Now all constraint set have been prepared, intesecting...";
            if( !ps.empty()) {
             //   remove_space_dimensions(index, sys.vars, ps);
                sys.poly.intersection_assign(ps);
            }                                
            cout << "... completed!" << endl;
        }
	}
//	sys.vars = update_var_names(index, sys.vars);
//    using namespace PPL::IO_Operators;
//    PPL::IO_Operators::operator << (std::cout, sys.poly);// << endl;
//    for (unsigned char i=0; i<sys.vars.size(); i++) {
//        char c = 'A' + i;
//        cout << c << ": " << sys.vars[i] << endl;
//    }

    return sys;
}
template<class IterInt, class IterTask> vector< vector<int> > 
SensitivityBuilder::number_of_instances( IterInt pb, IterInt pe, 
                                            IterTask tb, IterTask te) 
{
    vector< vector<int> > vect;
    for (IterInt p = pb; p!= pe; ++p) {
        vector<int> n;
        for (IterTask t = tb; t!=te; ++t) {
            int tag1 = t->get_pipeline_tag();
            int tag2 = te->get_pipeline_tag();
	    	if( tag1 == tag2 && is_a_pline_task(*te)) {
	    		int ceil = 0;
                double dline = t->get_dline();
                double period = t->get_period();
				if( dline > period) {
					ceil = (dline - period)/period;
					if( ((int)(dline - period)%(int)(period) != 0))
						ceil += 1; 
				}
				int x = *p / period;
				if (*p % (int)period) x++;
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

template<class Iter>
int SensitivityBuilder::compute_max_blocking_time(Iter b, Iter e)
{
	int B_i = 0;
	int tmp;
	for (Iter i=b+1; i != e; i++) {
        int tag_i = i->get_pipeline_tag();
        int tag_b = b->get_pipeline_tag();
        double i_period = i->get_period();
        double i_dline = i->get_dline();
		if (tag_i == tag_b && is_a_pline_task(*i)) {
			tmp = 0;
			if ( i_dline > i_period) 
				tmp = min((int)(i_period), (int)(i_dline - i_period));
		}
		else tmp = min((int)i_period, (int)(i_dline));
		if( B_i < tmp)
			B_i = tmp;
	}
	return B_i;
}

void SensitivityBuilder::remove_space_dimensions( const int index, 
        const vector<string> &vars, PPL::Pointset_Powerset<C_Polyhedron> &poly) 
{
	const vector<string> &vars_list = vis->vars_list;
	vector<FPTask> v;
	for( auto x : vis->node[index].get_rq())
		v.push_back(*x);
	PPL::Variables_Set vars_;

	for( int i = 0; i < vars.size(); i++) {
		// If the i-th variable is in the vars_list, simply continue.
		if( get_index(vars_list, vars[i]) != -1) continue;

        // Now, the i-th variable is not in the vars_list.
		vector<string> ss = split(vars[i], ".");
		int ti = find_task(v, ss[0]);
		if( ti == -1) throw ("task not found\n");
		/**
			1)  If a task is not in a pipeline, we can insantiate its variables
			    that are not listed in vars_list.
			2)  If a pipeline task's wcet is not listed in vars_list,
                we can also insantiate this parameter.
		*/
		if( v[ti].get_pipeline_pos() == -1 || ss[1]=="wcet") {
			double v_ = get_value_from_task(v[ti], ss[1]);
			Variable xx(i);
			Congruence cg = ((xx %= int(v_)) / 0);
			poly.refine_with_congruence(cg);
			vars_.insert(xx);
		}
	}
	/**
		Here we try to insantiate the initial jitter and last dline
        avriables of a pipeline.
	*/
	for( vector<Scan::FPTask>::const_iterator it = v.begin(); 
                                                it != v.end(); it++) {
		/**
            Since the initial jitter of a pipeline is 0, we insantiate
            it anyway (no matter it is listed in vars_list or not). 
        */
		if( it->get_pipeline_pos() == 0) {
			string jitter = it->get_name() + ".jitter";
			int pos = get_index(vars, jitter);
			if( pos == -1) throw ("Variable not found");
			PPL::Variable xj(pos);

			double v_ = it->get_jitter();
       		Congruence cg = ((xj %= 0) / 0);
        	poly.refine_with_congruence(cg);
			vars_.insert(xj);
		}
		/** 
            We insantiate the last deadline of a pipeline 
            if it is not listed in vars_list. 
        */
		if ( it->get_pipeline_pos() >= 0) {
			int tag = it->get_pipeline_tag();
            int p_it = it->get_pipeline_pos();
            int p_last = vis->pipelines[tag].pline_tasks.size() - 1;
			if(p_it != p_last)
				continue;
			string dline = it->get_name() + ".dline";
            /** 
                If the last dline is listed in vars_list, we cannot
                insantiate it, which is a free variable.
            */
			if( get_index(vars_list, dline) != -1 ) continue;
			int pos = get_index(vars, dline);
			if( pos == -1) throw ("Variable not found");
			PPL::Variable xd(pos);

			double v_ = it->get_dline();
       		Congruence cg = ((xd %= v_) / 0);
        	poly.refine_with_congruence(cg);
			vars_.insert(xd);
		}
	}
    /** To remove variables we just insantiate above. */
	poly.remove_space_dimensions(vars_);
}
/** 
    After removing non-free variables in a node, this method will
    be called to update variable names.
*/
vector<string> SensitivityBuilder::update_var_names (const int index, 
                                             const vector<string> &vars) 
{ 
    // res will be the resulted variable names after update
	vector<string> res;
	const vector<string> &vars_list = vis->vars_list;
	vector<FPTask> v;
	for( auto x : vis->node[index].get_rq())
		v.push_back(*x);

	for( int i = 0; i < vars.size(); i++) {
		vector<string> ss = split(vars[i], ".");
		int ti = find_task(v, ss[0]);
		if( ti == -1) throw ("task not found\n");

		// The i-th variable is listed in vars_list
		if( get_index(vars_list, vars[i]) != -1){
			/** 
                If it is not the initial jitter of a pipeline, it will
                be kept. (Initial jitter of a pipeline has already been 
                removed.) 
            */
			if( !(ss[1]=="jitter" && v[ti].get_pipeline_pos() == 0))
				res.push_back(vars[i]);
		}
        /** 
            The i-th variable is not listed in vars_list and
            its corresponding task is inside a pipeline.
        */
		else if ( is_a_pline_task(v[ti])) {
            /** A pipeline's initial jitter has been removed. " */
			if(ss[1]=="jitter" && v[ti].get_pipeline_pos() == 0)
				continue;
			int tag = v[ti].get_pipeline_tag();
			/** 
                The last pline of a pipeline has been removed if
                it is not in vars_list.
			*/
			if( (ss[1]=="dline" && v[ti].get_pipeline_pos() 
						== vis->pipelines[tag].pline_tasks.size()-1))
				continue;
            /** 
                The pipeline task's wcet has been removed if
                it is not in vars_list.
            */
			if(ss[1]=="wcet") continue;

			res.push_back(vars[i]);
		}
			
	}
	return res;	
}

void SensitivityBuilder::merge_pline_constraints(
            const vector<Scan::FPTask> &pline_tasks, ConstraintsSystem &sys)
{
	const vector<Scan::FPTask> &tasks = pline_tasks;
	vector<string> & vars_list = vis->vars_list;

	for( vector<Scan::FPTask>::const_iterator it = tasks.begin(); 
                                                it != tasks.end(); it++) {
		string dline = it->get_name() + ".dline";
		int pos = get_index(sys.vars, dline);
		if( pos == -1) continue;

		const Scan::Pipeline &jt = vis->pipelines[it->get_pipeline_tag()];
		const vector<Scan::FPTask_ptr> &vect = jt.pline_tasks;
		int i = it->get_pipeline_pos();
		if( i != vect.size() - 1) {
			Scan::FPTask &task = *vect[i+1];
			string jitter = task.get_name() + ".jitter";
			//if( get_index(vars_list, jitter) != -1) continue;
			int j = get_index(sys.vars, jitter);
			if( j == -1) { continue;} 
			PPL::Variables_Set set;
			PPL::Variable xd(pos);
			PPL::Variable xj(j);
			PPL::Constraint cs_jitter = (xd-xj<=0);//(xd <= xj);
            int pre_bytes = sys.poly.total_memory_in_bytes();
			sys.poly.add_constraint(cs_jitter);
//PPL::IO_Operators::operator << (std::cout, nodes[i]->poly);// 
//cout << endl;
            int after_bytes = sys.poly.total_memory_in_bytes();
            curr_bytes += after_bytes - pre_bytes;
            if( curr_bytes > max_bytes) {
                max_bytes = curr_bytes;
                cout << "max_bytes updated!\n";
            }
		    if( get_index(vars_list, dline) == -1) {
                set.insert(xd);
		    	sys.vars.erase(sys.vars.begin() + get_index(sys.vars, dline));
            }
            if( get_index(vars_list, jitter) == -1) {
			    set.insert(xj);
			    sys.vars.erase(sys.vars.begin() + get_index(sys.vars, jitter));
            }
            pre_bytes = sys.poly.total_memory_in_bytes();
			sys.poly.remove_space_dimensions(set);
            after_bytes = sys.poly.total_memory_in_bytes();
            curr_bytes += after_bytes - pre_bytes;
            if( curr_bytes > max_bytes) {
                max_bytes = curr_bytes;
                cout << "max_bytes updated!\n";
             }
		}
	}
}

}


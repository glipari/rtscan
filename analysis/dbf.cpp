#include <cmath>
#include <sstream>
#include <algorithm>
#include <boost/math/common_factor.hpp>

#include "dbf.hpp"

using namespace std;

#define DLINE_TOLERANCE      1e-8

namespace Scan {
    
    DemandBoundFunction::DemandBoundFunction() : f(), interval(0), period(0), print(pairlist)
    {
        f.push_back(point_t(0, 0));
    }

    DemandBoundFunction::DemandBoundFunction(const Task &t) :
    		f(), interval(0), period(t.get_period()), print(pairlist)
    {
        if (t.get_dline() <= t.get_period()) { 
            interval = period;
        }
        else {
            interval = t.get_dline();
        }
        f.push_back(point_t(0.0, 0.0));
        f.push_back(point_t(t.get_dline(), t.get_wcet(), t.get_id(), 1, t.get_id(), 1));
    };

    DemandBoundFunction::DemandBoundFunction(const DemandBoundFunction &d) :
    		f(d.f), interval(d.interval), period(d.period), print(d.print)
    {
    }

    void DemandBoundFunction::set_period(int p, double i)
    {
        if (period == 0) {
            period = p;
            interval = i;
        }
        else THROW_EXC(IllegalValue, "dbf::set_period(): Period already set!");
    }

    /** @todo check following point! to see if it is correct */
    void DemandBoundFunction::add_point(double t, double v, int tsk_1, int trn_1, int tsk_2, int trn_2)
    {
        // first of all, check that the time instant is greater than
        // all points in the map
        if (t < f[f.size()-1].first) {
            stringstream err;
            err << "dbf::add_point(): adding t " << t 
                << " which is smaller than last point "
                << f[f.size()-1].first << endl;
            throw IllegalValue(err.str());
        }
        double val = this->operator()(t);

        if (t > (interval + DLINE_TOLERANCE)) {
            if (val > v) {
            	DBGPRTLN("dbf::add_point(): warning, dominated point discarded");
            	DEBUGVAR(t);
				DEBUGVAR(val);
				DEBUGVAR(v);
            }
            else if (val < v) {
                stringstream err;
                err << "dbf::add_point(), adding a wrong value!\n";
                err << "t =   " << t << "   interval = " << interval << "\n";
                err << "val = " << val << "    v = " << v << endl;
                throw IllegalValue(err.str()); 
            }
            else {
            	DBGFILEL;
            	DBGPRTLN("dbf::add_point(): warning, point beyond interval");
            	DEBUGVAR(t);
            	DEBUGVAR(val);
				DEBUGVAR(v);
            }
        }
        else if (val < v) {
        	DBGFILEL;
        	DBGPRINT("succesfully adding t = " << t << "   v = " << v);
            f.push_back(point_t(t, v, tsk_1, trn_1, tsk_2, trn_2));
        }
        else {
        	DBGFILEL;
        	DBGPRTLN("dbf::add_point(): warning, dominated point discarded");
        	DEBUGVAR(t);
        	DEBUGVAR(val);
			DEBUGVAR(v);
        }
    }

    DemandBoundFunction & DemandBoundFunction::operator=(const DemandBoundFunction &d)
    {
        f = d.f;
        interval = d.interval;
        period = d.period;
        print = d.print;

        return *this;
    }
    
    double DemandBoundFunction::operator()(double t) const
    { 
        vector<point_t>::const_iterator i = f.begin();
        int rep = 0;
        double diff = 0;
        double c = 0;

        if (t > interval) {
            rep = (int)floor( (t - interval)/period ) + 1;
            t = t - rep * period;
            diff = this->operator()(interval) - this->operator()(interval - period);
            DBGFILEL;
			DBGPRINT("        dbf::operator() ");
            DEBUGVAR(diff);
        }

        while (i != f.end() && t > (i->first - DLINE_TOLERANCE)) {c = i->second; ++i;}

        return c + rep*diff;
    }

    std::vector<double> DemandBoundFunction::get_points() const
    {
        vector<point_t>::const_iterator i = f.begin();
        vector<double> p;
        for (i=f.begin(); i != f.end(); ++i) p.push_back(i->first);

        return p;
    }

    std::vector<double> DemandBoundFunction::get_values() const
    {
        vector<point_t>::const_iterator i = f.begin();
        vector<double> p;
        for (i=f.begin(); i != f.end(); ++i) p.push_back(i->second);

        return p;
    }


    bool operator<(const DemandBoundFunction::point_t &p1, const DemandBoundFunction::point_t &p2)
    {
        if (p1.first < p2.first - DLINE_TOLERANCE) return true;
        else if ((fabs(p1.first - p2.first) < DLINE_TOLERANCE) && (p1.second < p2.second)) return true;
        else return false;
    }

    DemandBoundFunction operator+(const DemandBoundFunction &d1, const DemandBoundFunction &d2)
    {
    	if (d1.period == 0) return d2;
    	else if (d2.period == 0) return d1;

    	// none of the two is null, so let's do the computation
        DemandBoundFunction final;
        
        final.period = boost::math::lcm(d1.period, d2.period);
        final.interval = ( (d1.interval - d1.period) > 
                           (d2.interval - d2.period) ) ? 
            (d1.interval - d1.period) : (d2.interval - d2.period);

        final.interval += 2*final.period;

        int rep1 = (final.interval - d1.interval) / d1.period;
        int rep2 = (final.interval - d2.interval) / d2.period;
                
        vector<DemandBoundFunction::point_t>::const_iterator i;
        int j;

        final.f.push_back(DemandBoundFunction::point_t(0.0, 0.0));
        
        i=d1.f.begin();

        // eliminate the first 0,0
        ++i;
        for (; i!=d1.f.end(); ++i) 
            for (j=0; j<rep1; j++) {
                double t = i->first + j * d1.period;
                //                double v = i->second + j * d1.last_value;
                //                double val = d1(t) + d2(t);
                final.f.push_back(DemandBoundFunction::point_t(t, d1(t) + d2(t),
                		i->tsk_tag_1, i->trn_tag_1, i->tsk_tag_2, j+1/*i->trn_tag_2*/));
            }

        i=d2.f.begin();

        // eliminate the first 0,0
        ++i;
        for (; i!=d2.f.end(); ++i) 
            for (j=0; j<rep2; j++) {
                double t = i->first + j * d2.period;
                final.f.push_back(DemandBoundFunction::point_t(t, d1(t) + d2(t),
                		i->tsk_tag_1, i->trn_tag_1, i->tsk_tag_2, j+1 /*i->trn_tag_2*/));
            }
        
        // now, let's sort
        sort(final.f.begin(), final.f.end());

        //eliminate duplicates...
        DemandBoundFunction fin2;
        fin2.set_period(final.period, final.interval);
  
        DemandBoundFunction::point_t last(0.0,0.0);
        for (i=final.f.begin(); i!=final.f.end(); ++i) {
            if (*i == last) continue;
            fin2.f.push_back(*i);
            last = *i;
        }
        
        return fin2;
    }

    
    DemandBoundFunction sup(const DemandBoundFunction &d1, const DemandBoundFunction &d2)
    {
        DemandBoundFunction final;
        
        if (d1.get_period() == 0 && d1.get_values().size() == 1) {
        	return d2;
        }

        final.period = boost::math::lcm(d1.period, d2.period);
        final.interval = ( (d1.interval - d1.period) > 
                           (d2.interval - d2.period) ) ? 
            (d1.interval - d1.period) : (d2.interval - d2.period);
        final.interval += 2*final.period;

        int rep1 = (final.interval - d1.interval) / d1.period;
        int rep2 = (final.interval - d2.interval) / d2.period;
                
        vector<DemandBoundFunction::point_t>::const_iterator i;
        int j;
        
        for (i=d1.f.begin(); i!=d1.f.end(); ++i) 
            for (j=0; j<rep1; j++) {
                double t = i->first + j * d1.period;
                double v = d1(t);
                final.f.push_back(DemandBoundFunction::point_t(t, v, i->tsk_tag_1, i->trn_tag_1, i->tsk_tag_2, i->trn_tag_2));
            }
        
        stable_sort(final.f.begin(), final.f.end());
        
        for (i=d2.f.begin(); i!=d2.f.end(); ++i) 
            for (j=0; j<rep2; j++) {
                double t = i->first + j * d2.period;
                double v = d2(t);
                if (final(t) < v) {
                    final.f.push_back(DemandBoundFunction::point_t(t, v, i->tsk_tag_1, i->trn_tag_1, i->tsk_tag_2, i->trn_tag_2));
                    stable_sort(final.f.begin(), final.f.end());
                }
            }

        //eliminate duplicates...
        DemandBoundFunction fin2;
        fin2.set_period(final.period, final.interval);
  
        DemandBoundFunction::point_t last(0.0,0.0);
        for (i=final.f.begin(); i!=final.f.end(); ++i) {
            //if (*i == last) continue;
        	if (i->first >= last.first && i->second <= last.second) continue;

        	if (i->first == last.first && i->second > last.second && fin2.f.size()>0) {
        		vector<DemandBoundFunction::point_t>::iterator l= fin2.f.end();
        		l--;
        		l->second = i->second;

        		last = *l;
        		continue;
        	}
            fin2.f.push_back(*i);
            last = *i;
        }
        
        return fin2;
    }

    std::ostream & operator<<(std::ostream &o, const DemandBoundFunction &d)
    {
        unsigned int i;

        switch(d.get_output()) {
        case DemandBoundFunction::gnuplot: 
            for (i=0; i<d.f.size(); ++i) {
                o << d.f[i].first << ", " << d.f[i].second << ", (" << d.f[i].tsk_tag_1
                  << ", " << d.f[i].trn_tag_1 << "), (" << d.f[i].tsk_tag_2
                  << ", " << d.f[i].trn_tag_2 << ")"  << endl;
            }
            break;
        case DemandBoundFunction::pairlist: 
            o << "{";
            for (i=0; i<d.f.size(); ++i) {
                o << "(" << d.f[i].first << ", " << d.f[i].second << "), ";
            }
            o << "}";
            break;
        case DemandBoundFunction::instants: 
            for (i=0; i<d.f.size(); ++i) {
                o << d.f[i].first << ", ";
            }            
            break;
        case DemandBoundFunction::values: 
            for (i=0; i<d.f.size(); ++i) {
                o << d.f[i].second << ", ";
            }            
            break;
        default:
            break;
        }
        return o;
    }

    bool DemandBoundFunction::check_sched(double alpha, double delta)
    {
        unsigned int i;

        for (i=0; i<f.size(); i++) {
            if (f[i].first <= delta) {
                if (f[i].second > 0) return false;
                else continue;
            }
            else {
                if (f[i].second > (f[i].first - delta) * alpha) 
                    return false;
            }
        }
        return true;
    }

    double DemandBoundFunction::compute_slope(double &alpha, double delta)
    {
        unsigned int i;
        double critical_point = period;
        double diff = operator()(interval+period) - operator()(interval);
        alpha = diff;  // it's a computation time, initially

        for (i=0; i<f.size(); i++) {
        	if (f[i].first <= delta && f[i].second > 0)
				throw DbfNoSlope("Delta is too large, no feasible slope");
        	if (f[i].first > delta) {
        		for (int j=0; j<100; j++) {
        			double u = (f[i].second + j * diff);
        			if ( (u * critical_point) > alpha * (f[i].first + j * period - delta)) {
        				alpha = u;
        				critical_point = f[i].first + j * period - delta;
        			}
        		}
        	}
        }
        alpha /= critical_point;
        return critical_point + delta;
    }
    
    std::vector<int> DemandBoundFunction::get_tsk_id() const
    {
        vector<point_t>::const_iterator i = f.begin();
        vector<int> p;
        for (i=f.begin(); i != f.end(); ++i) p.push_back(i->tsk_tag_1);

        return p;
    }

    std::vector<int> DemandBoundFunction::get_trn_id() const
    {
        vector<point_t>::const_iterator i = f.begin();
        vector<int> p;
        for (i=f.begin(); i != f.end(); ++i) p.push_back(i->trn_tag_1);

        return p;
    }


    bool operator<=(const DemandBoundFunction &small, const DemandBoundFunction &big)
    {
    	bool answer = true;
    	std::vector<double> sup_points = big.get_points();
    	for (unsigned int i=0; i<sup_points.size(); i++) {
    		answer &= big(sup_points[i]) >= small (sup_points[i]);
    	}

    	std::vector<double> inf_points = small.get_points();
    	for (unsigned int i=0; i<inf_points.size(); i++) {
    		answer &= big(inf_points[i]) >= small (inf_points[i]);
    	}

    	return answer;
    }
}

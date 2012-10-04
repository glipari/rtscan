#include <sstream>
#include <cmath>
#include <map>
#include <vector>
#include <string>

#include <models/task.hpp>

using namespace std;

namespace Scan {
    template<>
    int HasUniqueId<Task>::counter = 0;

    void Task::init_name_()
    {
        stringstream nn;
        nn << "AnonymousTask"; 
        name = string(nn.str());        
    }

    Task::Task() : period(0), wcet(0), dline(0) 
    {
        init_name_();
    }

    Task::Task(const string &n) : name(n), period(0), wcet(0), dline(0) 
    {   
    }

    Task::Task(double w, double d, int p, int off, double jit) 
        throw(IllegalValue) : period(p), wcet(w), dline(d), offset(off), jitter(jit)
    {
        if (w > p || w < 0 || p < 0 || d < 0) {
            stringstream err;
            err << "cannot create task with parameters w: " 
                << w << " p : " << p << " d : " << d;
            
            throw IllegalValue(err.str());
        }
        init_name_();
    }
    
    Task::Task(const Task &t) : HasUniqueId(t), name(t.name), period(t.period), wcet(t.wcet), dline(t.dline), offset(t.offset), jitter(t.jitter)
    {
    }

    Task & Task::operator=(const Task &t)
    {
        HasUniqueId::operator=(t);
        name = t.name;
        period = t.period;
        dline = t.dline;
        wcet = t.wcet;
        offset = t.offset;
        jitter = t.jitter;

        return *this;
    }

    int Task::get_period() const
    {
        return period;
    }
    
    double Task::get_wcet() const
    {
        return wcet;
    }

    double Task::get_dline() const
    {
        return dline;
    }

    void Task::set_period(int t) throw(IllegalValue)
    {
        if (wcet > t || t < 0) {
            stringstream err;
            err << "cannot set period p: " << t << " with wcet w: " << wcet;
            
            throw IllegalValue(err.str());
        }
        period = t;
    }

    void Task::set_wcet(double w) throw(IllegalValue)
    {
        if (w > period || w < 0) {
            stringstream err;
            err << "cannot set wcet w: " << w << " with period p: " 
                << period;             
            throw IllegalValue(err.str());
        }
        wcet = w;
    }

    void Task::set_dline(double d) throw(IllegalValue)
    {
        if (d < 0) {
            stringstream err;
            err << "cannot set dline d: " << d;
            
            throw IllegalValue(err.str());
        }
        dline = d;
    }

    std::ostream & operator<<(std::ostream &o, const Task &t)
    {
        o << t.get_wcet() << "  " << t.get_dline() << "  " << t.get_period(); 
        return o;
    }

    std::istream & operator>>(std::istream &i, Task &t) throw(IllegalValue)
    {
        double w, d;
        int p;
        i >> w >> d >> p;

        t.set_period(p); t.set_dline(d); t.set_wcet(w); 
        return i;
    }

    using namespace std;


    template<class T>
    bool check_value(const map<string, T> &m, const string &n)
    {
        return m.find(n) != m.end();
    }

    template <class T>
    void check_and_store_value(map<string, T> &m, const string &n, T v)
    {
        if (check_value(m, n)) THROW_EXC(ValueAlreadySet, "value already set");
        else m[n] = v;
    }
    
    template <class T> 
    void store_value(map<string, T> &m, const string &n, T v)
    {
        m[n] = v;
    } 


    struct TaskVisitor : public boost::static_visitor<> 
    {
        map<string, double> d_values;
        map<string, int> i_values;
        vector< vector<string> > md_keys;
        vector< vector<string> > od_keys;
        vector< vector<string> > mi_keys;
        vector< vector<string> > oi_keys;

        string name;
        string type;
        
        TaskVisitor() : name(""), type("") {
            md_keys.push_back({"c", "C", "wcet"}); 
            od_keys.push_back({"d", "D", "dline"}); 
            od_keys.push_back({"j", "J", "jitter"}); 
            mi_keys.push_back({"p", "T", "period"}); 
            oi_keys.push_back({"o", "O", "offset"}); 
        }
        
        void operator()(const PropertyList &plist) {
            // nothing to do right now
            name = plist.name;
            type = plist.type;
            for (auto &x : plist.children) 
                boost::apply_visitor(std::ref(*this), x);
        }
        
        string check_keyword(const vector< vector<string> > &v, const string &name) {
            for (auto const &x : v) {
                string k = x[0];
                for (auto const &y : x)
                    if (name ==  y) return k;
            } 
            return "";
        }

        void operator()(const Property &p) {
            // if the p.name is equal to one of the keywords, then 
            // store that in the appropriate data structure
            string k = check_keyword(md_keys, p.name);
            if (k != "") check_and_store_value(d_values, k, p.get_double());
            else {
                k = check_keyword(od_keys, p.name);
                if (k != "") check_and_store_value(d_values, k, p.get_double());
                else {
                    k = check_keyword(mi_keys, p.name);
                    if (k != "") check_and_store_value(i_values, k, p.get_int());
                    else {
                        k = check_keyword(oi_keys, p.name);
                        if (k != "") check_and_store_value(i_values, k, p.get_int());
                    }
                }
            }
        }

        Task create_task() {
            if (check_value(d_values, "d")) d_values["d"] = d_values["p"];
            if (check_value(d_values, "j")) d_values["j"] = 0;
            if (check_value(d_values, "o")) d_values["o"] = 0;
            
            Task t(d_values["c"], 
                   d_values["d"],
                   i_values["p"], 
                   d_values["j"], 
                   d_values["o"]);
            t.set_name(name);
            return t;
        }
    };

    Task create_task(const PropertyList &p)
    {
        TaskVisitor vis;
        vis(p);
        return vis.create_task();
    }



}

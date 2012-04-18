#include <algorithm>
#include <models/task.hpp>
#include <models/has_critical_section.hpp>
#include <analysis/fmlp.hpp>
#include <common/sequence.hpp>
#include <stack>

namespace Scan {
    using namespace std;
        
    void fmlp_add_to_group(ResGroup &g, const Resource& r)
    {
        g.insert(r);
    }

    void fmlp_create_group(ResGroupSet &groups, const Resource& r)
    {
        ResGroup v;
        fmlp_add_to_group(v, r);
        groups.push_back(v);
    }

    int fmlp_find_group(const ResGroupSet &groups, const Resource& res) 
    {
        for (unsigned i=0; i<groups.size(); i++) {
            if (find_if(groups[i].begin(), groups[i].end(), ResourceIdEqualPred(res)) != groups[i].end()) return i;
        }
        return -1;
    }

    int fmlp_find_group(const ResGroupSet &groups, int id) 
    {
        for (unsigned i=0; i<groups.size(); i++) {
            if (find_if(groups[i].begin(), groups[i].end(), ResourceIdEqualPred(id)) != groups[i].end()) return i;
        }
        return -1;
    }
        
    bool fmlp_task_uses_group(const HasCriticalSection &task, const ResGroup &g)
    {
        for (auto i = g.begin(); i != g.end(); ++i ) 
            if (task.uses_resource(i->get_id())) return true; 
        
        return false;
    }
    
    
    void FMLPAnalysis::fmlp_compute_group_recursive(int parent_res_id, CSSet::const_iterator begin, CSSet::const_iterator end)
    {
        for (CSSet::const_iterator i=begin; i!=end; ++i) {
            int rid = i->get_resource();
            Resource r = get_res(rid);
            bool res_isshort = r.isShort();
            bool parent_isshort = false;
            if (parent_res_id != -1) parent_isshort = get_res(parent_res_id).isShort();
            if (parent_res_id == -1 || parent_isshort != res_isshort) {
                int g_index = fmlp_find_group(gs, r);
                if (g_index == -1) 
                    fmlp_create_group(gs,r);
            }
            else {
                // parent_res_id == 1 AND parent_isshort == res_isshort
                int g_index = fmlp_find_group(gs, rid); 
                int g_parent = fmlp_find_group(gs, parent_res_id);
                if (g_parent == -1) throw "Parent not in a group!!!";
                if (g_index == -1) fmlp_add_to_group(gs[g_parent], r);
                else if (g_parent != g_index) {
                    // merging
                    gs[g_parent].insert(gs[g_index].begin(), gs[g_index].end());
                    gs.erase(gs.begin()+g_index); // erase second group 
                }
                // if g_parent == g_index, do nothing, and go deeper
            }
            fmlp_compute_group_recursive(rid, i->begin(), i->end());
        } 
    }

    ResGroupSet FMLPAnalysis::compute_groups()
    {
        gs.clear();
        for (auto t : tset) {
            CSSet s = t.get_outer_cs();
            fmlp_compute_group_recursive(-1, s.begin(), s.end());
        }
        return gs;
    }

    double FMLPAnalysis::compute_spin(const TaskRes &task, const Resource &r) 
    {
        int group = fmlp_find_group(gs, r);
        // given a task, I need to know its longest critical on resource r.
        vector <double> v;
        for (auto t : tset) {
            if (t.get_id() == task.get_id()) continue;
            double m = 0.0;
            for (auto rid : gs[group]) {
                double l = t.get_max_length(rid.get_id());
                if (l>m) m = l;
            }
            v.push_back(m);
        }
        return sum_max_n(v.begin(), v.end(), num_proc-1);
    }

    CSSet FMLPAnalysis::short_outermost_cs(CSSet::const_iterator begin, CSSet::const_iterator end) const
    {
        stack<CriticalSection> s;
        for (auto i = begin; i != end; ++i) s.push(*i);
        CSSet work;
        while(s.size() >0) {
            CriticalSection cs = s.top();
            s.pop();
            Resource r = get_res(cs.get_resource());
            if (r.isShort()) work.push_back(cs);
            else 
                for (auto i = cs.begin(); i!=cs.end(); ++i) s.push(*i);
        }

        return work;
    }

    CSSet FMLPAnalysis::long_outermost_cs(const TaskRes &task) const
    {
        CSSet csset = task.get_outer_cs();
        CSSet work;
        for (auto cs : csset) {
            Resource r = get_res(cs.get_resource());
            if (!r.isShort()) work.push_back(cs);
        }
        return work;
    }


    double FMLPAnalysis::non_preemptable(const TaskRes &task)
    {
        CSSet my = task.get_outer_cs();
        CSSet work = short_outermost_cs(my.begin(), my.end());
        double m = 0.0;
        for (auto cs : work) {
            Resource r =  get_res(cs.get_resource());        
            double l = cs.get_duration() + compute_spin(task, r);
            m = max(m, l);
        }
        return m;
    }

    double FMLPAnalysis::max_np_other(const TaskRes& task)
    {
        double m2 = 0.0;
        for (auto t: tset) 
            if (t.get_id() != task.get_id()) m2 = max(m2, np[t.get_id()]);
        return m2;
    }

    double FMLPAnalysis::max_np_other2(const TaskRes& task1, const TaskRes& task2)
    {
        double m2 = 0.0;
        for (auto t: tset) 
            if ((t.get_id() != task1.get_id()) && (t.get_id() != task2.get_id())) 
                m2 = max(m2, np[t.get_id()]);
        return m2;
    }
    
    void FMLPAnalysis::compute_np()
    {
        np.clear();
        for (auto t: tset) 
            np[t.get_id()] = non_preemptable(t);
    }

    double FMLPAnalysis::compute_np_blocking(const TaskRes &task)
    {
        compute_np();

        vector<TaskRes> gperiod;
        select(gperiod, tset.begin(), tset.end(), [task](const TaskRes &x) { return x.get_period() > task.get_period(); });

        double m1 = 0.0;
        for (auto t: gperiod) m1 = max(m1, np[t.get_id()]);

        double m2 = max_np_other(task);
        
        int l = long_outermost_cs(task).size();
        return m1 + l * m2;
    }

    double FMLPAnalysis::holding_time(const TaskRes &task, const CriticalSection &req)
    {
        // of course, req must be a long resource
        double tot = req.get_duration();
        CSSet work = short_outermost_cs(req.begin(), req.end());
        for (auto cs : work) {
            Resource r = get_res(req.get_resource());
            tot += compute_spin(task, r);
        } 
        return tot;
    }

    Resource FMLPAnalysis::get_res(int rid) const
    {
        return get_res_by_id(rid, rset.begin(), rset.end());
    }
    
    double FMLPAnalysis::direct_blocking_on_resource(const TaskRes &task, const Resource &res)
    {
        set<TaskRes, LessUniqueId> tgroup;
        int gindex = fmlp_find_group(gs, res.get_id());
        for (Resource x : gs[gindex]) {
            for (TaskRes t : tset) if (t.uses_resource(x.get_id())) tgroup.insert(t); 
        }
        double tot = 0.0;
        // now I have in tgroup all tasks that use a resource in group gindex
        for (TaskRes t : tgroup) { 
            // the first contribution is max_np_other2(t, task);
            
            // now the second contribution
            double m2 = 0.0;
            CSSet work = long_outermost_cs(t);
            for (auto cs2 : work) 
                m2 = max(m2, holding_time(t, cs2));
            
            tot += max_np_other2(t, task) + m2;
        }
        return tot;
    }

    double FMLPAnalysis::compute_direct_blocking(const TaskRes &task)
    {
        CSSet lset = long_outermost_cs(task);
        double tot = 0.0;
        for (auto cs : lset) 
            tot += direct_blocking_on_resource(task, get_res(cs.get_resource()));
                
        return tot;
    }    

    double FMLPAnalysis::blocking_time(const TaskRes &task)
    {
        compute_groups();
        compute_np();

        // spin blocking time
        CSSet all = task.get_outer_cs();
        CSSet csset = short_outermost_cs(all.begin(), all.end());
        double spin = 0.0;
        for (auto cs : csset)
            spin += compute_spin(task, get_res(cs.get_resource()));
        
        return spin + compute_np_blocking(task) + compute_direct_blocking(task);
    }

}

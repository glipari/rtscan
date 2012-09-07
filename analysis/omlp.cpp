#include "omlp.hpp"
#include <common/sequence.hpp> 

namespace Scan  {
    using namespace std;

    void compute_tifs(vector<double> &lengths, const vector<TaskRes> &localset,
                      int rid, double interval, int niq)
    {
        for (auto t : localset) {
            double m = 0;
            int n = 0;
            CSSet csset = t.get_outer_cs();
            for (auto cs : csset) {
                if (cs.get_resource() == rid) { 
                    m = max(m, cs.get_duration());
                    n++;
                }
            }
            int ntif = n*ceil((interval + t.get_dline())/t.get_period());
            for (int i=0;i<min(ntif, niq);++i) 
                lengths.push_back(m);
        }
    }

    double OMLPAnalysis::blocking_time_global(const TaskRes &task)
    {
        double block = 0;
        vector<TaskRes> lset; 
        select(tset.begin(), tset.end(), back_inserter(lset),
               [&task](const TaskRes &t2) {return t2.get_id() != task.get_id();});
        
        // I will use the deadline instead of the response time, for the moment
        CSSet csset = task.get_outer_cs();
        // read the critical section, map r_id with the number of critical sections 
        // that access it.
        map<int, int> Ncs;
        for (auto cs : csset) {
            int rid = cs.get_resource();
            if (Ncs.find(rid) == Ncs.end()) 
                Ncs[rid] = 1;
            else Ncs[rid]++;
        }
        

        for (auto r : Ncs) { 
            int rid = r.first;
            int Niq = r.second;
            
            vector<TaskRes> localset;
            subset_tasks_use_res(lset.begin(), lset.end(), 
                                 back_inserter(localset), rid);
            int Aq = localset.size() + 1;
            int xq, lq;
            if (Aq <= num_proc+1) {
                xq = Aq; lq = 1;
            }
            else {
                xq = 2 * num_proc; lq = 2;
            }            

            vector<double> tifs;
            compute_tifs(tifs, localset, rid, task.get_dline(), lq*Niq);
            block += sum_max_n(tifs.begin(), tifs.end(), (xq-1)*Niq);
        }
        
        return block;
    }
}

#ifndef __MEDF_RTA_HPP__
#define __MEDF_RTA_HPP__

#include <vector>
#include <algorithm>
#include <models/task.hpp>
#include <models/has_slack.hpp>
#include <analysis/fp_response_time.hpp>

namespace Scan {
    bool gedf_iterative(int n_proc, std::vector<Task> &task);

    typedef TaskEx<HasSlack> MEDF_Task;

    /** 
        Helper function for gedf_iterative()
        
        It computes the interference of task ti on task tk
    */
    template<class TaskType>
    double gedf_interf_i_k(const TaskType &tk, const TaskType &ti, int n_proc)
    {
        int n = int( floor( double(tk.get_dline()) / double(ti.get_period()) ) );
        double x =  std::max(0.0, (tk.get_dline() - ti.get_slack() - n * ti.get_period() ) );
        return n * ti.get_wcet() +  std::min(ti.get_wcet(), x); 
    }

    /**
       Helper function for gedf_iterative()

       It computes a lower bound on the slack of task tk.
     */
    template<class TaskType, class Iter> 
    double gedf_slack_k(const TaskType &tk, Iter tset_begin, Iter tset_end, int n_proc)
    {
        double sum = 0.0;
        for (auto ti = tset_begin; ti != tset_end; ++ ti) 
            if (ti->get_id() != tk.get_id()) 
                sum += std::min(gedf_interf_i_k(tk, *ti, n_proc), tk.get_dline() - tk.get_wcet() + 1);
        
        return tk.get_dline() - tk.get_wcet() - std::floor( sum / double(n_proc) );   
    }

    /** 
        RTA Analysis of GEDF.
      
        M. Bertogna, M. Cirinei, and G. Lipari, “Schedulability
        Analysis of Global Scheduling Algorithms on Multiprocessor
        Platforms,” IEEE Transactions on Parallel and Distributed
        Systems, vol. 20, no. 4, pp. 553-566, Apr. 2009.
    */
    template <class Iter>
    bool gedf_iterative(int n_proc, Iter v_begin, Iter v_end)
    {
        std::vector<MEDF_Task> tset(v_begin, v_end);
        bool stop = false;
        bool feasible = false;
        
        for (auto k = tset.begin(); k != tset.end(); ++k) 
            k->set_slack(0);

        while (!stop && !feasible) {
            stop = true;
            feasible = true;
            for (auto k=tset.begin(); k != tset.end(); ++k) {
                double sb = gedf_slack_k(*k, tset.begin(), tset.end(), n_proc);
                if (sb < 0) feasible = false;
                if (sb > k->get_slack()) {
                    k->set_slack(sb);
                    stop = false;
                }
            }
        }
        // exits because stop = true (no task updated) or because
        // feasible = true (all tasks are schedulable). Notice that if
        // some sb < 0 and stop = true, the system is deemed not
        // schedulable
        return feasible;
    }


    /**
       Fixed priority schedulability test based on density bound.

       J. Goossens, S. Funk, and S. Baruah, “Priority-driven
       scheduling of periodic task systems on multiprocessors,” Real
       Time Systems, vol. 25, no. 2–3, pp. 187–205, 2003.
    */
    double lambda(const Task &t)
    {
        return (t.get_wcet()/t.get_dline());
    }

    template <class Iter>
    bool gedf_bound_gfb(int n_proc, Iter tset_begin, Iter tset_end)
    {
	double beta_tot=0, lambda_max=0, lim=0;
        
	for(auto k=tset_begin; k != tset_end; k++) {
            beta_tot += lambda(*k);

            if (lambda(*k) > lambda_max){
                lambda_max = lambda(*k);
                lim = n_proc*(1-lambda_max)+lambda_max;
            }
            
            if (beta_tot > lim)
		return false;
	}
	return true;
    }

    /**
       GEDF schedulability test, by Goossens, Funk, Baruah (the GFB test).
       
       J. Goossens, S. Funk, and S. Baruah, “Priority-driven
       scheduling of periodic task systems on multiprocessors,” Real
       Time Systems, vol. 25, no. 2–3, pp. 187–205, 2003.
     */
    template <class Iter>
    bool gfp_bound_density(int n_proc, Iter tset_begin, Iter tset_end)
    {
        double beta_tot=0, lambda_max=0, lim=0;
        //vector<FPTask> tset(tsetin);

        sort_by_increasing(tset_begin, tset_end, &Task::get_dline);

        for(auto k=tset_begin; k != tset_end; k++) {
            beta_tot += lambda(*k);
            
            if (lambda(*k) > lambda_max){
                lambda_max = lambda(*k);
                lim = ((double)n_proc / 2.0)*(1 - lambda_max)+lambda_max;
            }
            
            if (beta_tot > lim)
                return false;
        }
        return true;
    }
}

#endif

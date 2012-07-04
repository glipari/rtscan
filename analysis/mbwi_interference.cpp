#include <functional>

#include <common/sequence.hpp>
#include "mbwi_interference.hpp"

namespace Scan {
    using namespace std;

    double calc_permutation(const vector<TaskRes>& taskset,
                            const vector<TaskRes>& valid_tasks, 
                            const vector<TaskRes>& blocked_tasks, 
                            vector<int> local_blocked_res,
                            int r_k)
    {
        double perm_sum = 0.0;
        for (auto task_j : valid_tasks) {
            vector<TaskRes> local_blocked_tasks(blocked_tasks);
            local_blocked_tasks.push_back(task_j);
                    
            double max_length = 0;
            vector<int> max_temp_blocked_res;

            // all critical section of task_j on r_k
            CSList cslist = task_j.get_cs_list(r_k);
            auto cs_iter2 = cslist.begin();
            for (; cs_iter2 != cslist.end(); cs_iter2++) {
                CSList path = (*cs_iter2)->get_path();
                bool flag = false;
                for (auto rr : local_blocked_res) {
                    if (find_cs(path.begin(), path.end(), rr) != path.end()) {
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    // the cs is good!
                    // push all resources locked along the path
                    vector<int> temp_blocked_res(local_blocked_res);
                    for (auto rb : path) 
                        temp_blocked_res.push_back(rb->get_resource());
                    
                    double temp = (*cs_iter2)->get_duration() + 
                        mbwi_compute_interference(taskset,
                                                  local_blocked_tasks,
                                                  temp_blocked_res,
                                                  (*cs_iter2)->begin(),
                                                  (*cs_iter2)->end());
                    
                    if (max_length < temp) {
                        max_length = temp;
                        max_temp_blocked_res = temp_blocked_res;
                    }
                }
            }
            perm_sum += max_length;
            local_blocked_res = max_temp_blocked_res;
        }
        return perm_sum;
    }

    vector<TaskRes> compute_gamma(const vector<TaskRes>& taskset, int r_k)
    {
        using namespace std::placeholders;

        // compute gamma_k (set of tasks that access r_k)
        auto mf = std::bind(std::mem_fn(&TaskRes::uses_resource), _1, r_k);
        vector<TaskRes> gamma_k;
        select(taskset.begin(), taskset.end(), back_inserter(gamma_k),
               mf);
        
        return gamma_k;
    }

    
    double mbwi_compute_interference(const vector<TaskRes>& taskset, 
                                     const vector<TaskRes>& blocked_tasks, 
                                     const vector<int>& blocked_res, 
                                     CSSet::const_iterator csset_begin,
                                     CSSet::const_iterator csset_end)
    {
        double sum = 0;

        for (auto cs_iter = csset_begin; cs_iter != csset_end; cs_iter++) {
            int r_k = cs_iter->get_resource();

            vector<int> local_blocked_res(blocked_res);
            local_blocked_res.push_back(r_k);

            vector<TaskRes> gamma_k = compute_gamma(taskset, r_k);
            
            // eliminate those tasks that are already in the blocking
            // chain
            vector<TaskRes> valid_tasks;
            subtract(gamma_k.begin(), gamma_k.end(),
                     blocked_tasks.begin(), blocked_tasks.end(),
                     back_inserter(valid_tasks),
                     task_cmp_ids
                );
            // now I have the list of tasks that could potentially
            // block the task under analysis on r_k

            // sort them, according to the id ordering
            sort(valid_tasks.begin(), valid_tasks.end(), LessUniqueId());

            // compute all possible permutations of valid_tasks
            // then compute a separate sum for each possible 
            // permutation
            // finally, compute the maximum of all the sums, and 
            // add it to the global sum
            double max_perm_sum = 0.0;
            bool flag = true;
            while (flag) {
                max_perm_sum = max(max_perm_sum, 
                                   calc_permutation(taskset, valid_tasks, blocked_tasks, local_blocked_res, r_k));
                flag = next_permutation(valid_tasks.begin(), valid_tasks.end(), LessUniqueId());
            }
            sum += max_perm_sum;

            // I have computed the interference on the outermost 
            // critical section. Now, I have to suppose that I take 
            // the CS, and block on the inner ones. 
            // r_k is already in the list of locked resources (this time 
            // by the task under analysis).
            // hence, I call the same function on the nested critical sections, 
            // of this one, if any
            sum += mbwi_compute_interference(taskset,
                                             blocked_tasks,
                                             local_blocked_res,
                                             cs_iter->begin(),
                                             cs_iter->end());
            
        } 
        return sum;
    }


    double mbwi_compute_interference(const TaskRes& task,
                                     vector<TaskRes> taskset)
    {
        CSSet cs_set = task.get_outer_cs();
        vector<int> bres; 
        vector<TaskRes> btask;
        btask.push_back(task);
        return mbwi_compute_interference(taskset, btask, bres, cs_set.begin(), cs_set.end());
    }
}

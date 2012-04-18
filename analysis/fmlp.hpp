#ifndef __FMLP_HPP__
#include <vector>
#include <set>
#include <map>
#include <models/taskres.hpp>

namespace Scan {

    typedef std::set<Resource, ResourceIdLessThan> ResGroup;
    typedef std::vector<ResGroup> ResGroupSet;
    
    class FMLPAnalysis {
        int num_proc;
        std::vector<TaskRes> tset;
        std::vector<Resource> rset;
        std::map<int, double> np;

        void fmlp_compute_group_recursive(int parent_res_id, 
                                          CSSet::const_iterator begin, 
                                          CSSet::const_iterator end);
        
        ResGroupSet gs;
    public:
        template<class TaskIter, class ResIter>
        FMLPAnalysis(int nproc, TaskIter tset_begin, TaskIter tset_end, ResIter res_begin, ResIter res_end) :
            num_proc(nproc),
            tset(tset_begin, tset_end),
            rset(res_begin, res_end),
            gs()
            {
                //compute_groups();
            }
        
        double blocking_time(const TaskRes &task);

    protected:
        Resource get_res(int res_id) const;
        CSSet long_outermost_cs(const TaskRes &task) const;
        CSSet short_outermost_cs(CSSet::const_iterator begin, CSSet::const_iterator end) const;

        // computes resource groups, to be called at the beginning (maybe in the constructor)
        ResGroupSet compute_groups();
        
        // computes the longest non-preemptable section for task 
        double non_preemptable(const TaskRes &task);

        // computes the longest non-preemptable sections for all tasks
        // and stores in a map. To be called after compute groups
        void compute_np();

        // returns the max np section by tasks other than task
        double max_np_other(const TaskRes& task);

        // returns the max np section by tasks other than task1 and task2
        double max_np_other2(const TaskRes& task1, const TaskRes& task2);

        // computes the spin suffered by task on resource r
        // Assumes that task uses r, but does not check it 
        // (This should become private or protected ... but then, how I can test it?)
        double compute_spin(const TaskRes &task, const Resource &r);
        
        // computes the non preemptable blocking contribution
        double compute_np_blocking(const TaskRes &task);
        
        // computes the holding time of task on critical section req (which is a long cs)
        double holding_time(const TaskRes &task, const CriticalSection &req);

        // direct blocking on resource res (which is a long cs)
        double direct_blocking_on_resource(const TaskRes &task, const Resource &res);

        // computes the direct blocking contribution
        double compute_direct_blocking(const TaskRes &task);
    };
}
#endif

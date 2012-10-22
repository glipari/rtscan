#ifndef PROC_ALLOCATION_INCLUDED
#define PROC_ALLOCATION_INCLUDED

#include <models/Stage_Set.hpp>
#include <models/Processor.hpp>

namespace Scan {
class Procs_Allocation
{
    /** contains tasks set of pipline allocate on processor proc;
        task_x_pipe[i] is the set of task of pipeline i allocated
        on processor proc
    */
    vector<Stage_Set> task_x_pipe;
    /** contains bandwidth occupied by tasks set of pipline allocate on
        processor proc;
        bw_per_pipe[i] is the bandwidth needed to allocate set of task
        of pipeline i;
    */
    vector <double> bw_per_pipe;
    Processor proc;
    public:
    Procs_Allocation();
     /**
           Constructor of Stage.
           @param p: processor in which tasks of pipeline are allocated;
           @param index_pipe: index of first pipeline that is allocated
                    on the processor p(all or some tasks of them) ;
           @param r_set: set of tasks of pipeline at index index_pipe;
           @param bw: bandwith needed to allocate the tasks set r_set;
    */
    Procs_Allocation(Processor p, int index_pipe,Stage_Set r_set,double bw);
    /** Copy constructor */
    Procs_Allocation(const Procs_Allocation & p);
    /** return processor proc */
    Processor get_Processor() { return proc;};
    int size(){return task_x_pipe.size();};
    Stage_Set get_at(int index);
    void update_task_allocated(Processor p, int index_pipe,Stage_Set r_set, double bw);
    void print();
    /** return true if the id of processor proc is equal to parameter */
    bool is_processor(int id);
    void remove_allocation_pipe(int index_pipe, Processor p);
    double get_bw_per_pipe(int index_pipe);
    Procs_Allocation& operator=( const Procs_Allocation &p);
};



/****** Feasible Allocation structure ******/


class Feasible_Allcoation
{
        /** contains all allocation for each processor.
        **/
        vector <Procs_Allocation> allocation_per_processor;
public:
    Feasible_Allcoation();
    Feasible_Allcoation(vector <Procs_Allocation> list);
    Feasible_Allcoation(const Feasible_Allcoation &f);
    int size(){return allocation_per_processor.size();};
    Procs_Allocation  get_at(int index);
    vector<Procs_Allocation> get_vector_allocation();
    void add(vector<Procs_Allocation>v);
    void insert(Procs_Allocation p, int index);
    void push_back(Procs_Allocation pa);
    void print();

};

std::ostream &operator<<(ostream &s, Stage_Set &r);

std::ostream &operator<<(ostream &s, Procs_Allocation &p);


std::ostream &operator<<(ostream &s, Feasible_Allcoation &fa);

}

#endif // PROC_ALLOCATION_INCLUDED

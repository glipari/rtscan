#ifndef PROC_ALLOCATION_INCLUDED
#define PROC_ALLOCATION_INCLUDED

#include <models/Stage_Set.hpp>
#include <models/Processor.hpp>

namespace Scan {
class Procs_Allocation
{
    vector<Stage_Set> task_x_pipe;
    Processor proc;
    public:
    Procs_Allocation();
    Procs_Allocation(Processor p, int index_pipe,Stage_Set r_set);
    Procs_Allocation(const Procs_Allocation & p);
    Processor get_Processor() { return proc;};
    int size(){return task_x_pipe.size();};
    Stage_Set get_at(int index);
    void update_task_allocated(Processor p, int index_pipe,Stage_Set r_set);
    void print();
    bool is_processor(int id);

};



/****** Feasible Allocation structure ******/


class Feasible_Allcoation
{
        vector <Procs_Allocation> feasible_comb;
public:
    Feasible_Allcoation();
    Feasible_Allcoation(vector <Procs_Allocation> list);
    Feasible_Allcoation(const Feasible_Allcoation &f);
    int size(){return feasible_comb.size();};
    Procs_Allocation  get_at(int index);

    void add(vector<Procs_Allocation>v);
    void push_back(Procs_Allocation pa);
    void print();

};

std::ostream &operator<<(ostream &s, Stage_Set &r);

std::ostream &operator<<(ostream &s, Procs_Allocation &p);


std::ostream &operator<<(ostream &s, Feasible_Allcoation &fa);

}

#endif // PROC_ALLOCATION_INCLUDED
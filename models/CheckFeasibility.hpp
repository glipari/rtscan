#ifndef CHEACKFEASIBILITY_HPP_INCLUDED
#define CHEACKFEASIBILITY_HPP_INCLUDED


#include <map>
#include <algorithm>
#include <models/task.hpp>
#include <models/Processor.hpp>
#include <models/taskset.hpp>
#include <models/Stage.hpp>
#include <models/Proc_Allocation.hpp>
#include <sstream>
#include <fstream>



namespace Scan
{

enum orderTaskSelection { precedence, decrease_wcet, increase_wcet };
enum precSelectionAlg { bestFit, worstFit, firstFit };

   /// sort processor_utilised

template<class Cmp>
void sort_proc(vector<Processor> proc)
{
    std::sort(proc.begin(),proc.end(), Cmp());
}
    /*** check if pipes are feasible */
class CheckFeasibility
{

    vector<vector<int>> positions_combinations;
    vector <Feasible_Allcoation> feasible_all;
    vector< pair<double,double> >period_deadline;
    int verbose;
    /* type of algorithm used to select taks*/
    orderTaskSelection order_task;
    precSelectionAlg proc_algorithm;
    vector<Processor> processors;
    vector <TaskSet> task_sets;
    ofstream output_file;

    void print_feasible_combination();
    int select_Processor(vector<Processor> *proc,  double sigma);
    double compute_partialBW(vector<Stage_Set>sets);
    /** Given a combinations, return tasks of pipe grouping
     according split combination
     @param position: is a combination (for example
    a combination 1 2 2 1 show that firsth and fourth task
    are allocated in one processor and secondth and thirth
    are allocated in another
    @param index_pipe : index of pipe that we considering**/
    vector <Stage_Set> get_Stage_Set(vector <int> position, int index_pipe);

    /** Generate number of tasks between num_proc and 3*num_proc
    where num_proc is a variable number of processor
     @param size_pipe size of pipe
     @param index index of task**/
    vector<int> get_combination(int size_pipe,int index);
    /** return true combination of task at index "index"
    is the last of combination to check if is feasible
         @param size_pipe size of pipe
         @param index of combination of task
         @param max number of group in which pipe tasks are splited**/
    bool is_last_combination(int index, int size, int max_split);
    void printProcessor(vector< Processor>p);
    bool check_feasibility_combinations( vector<Processor> *pro,int index_pipe,int max_split,vector<Procs_Allocation  >one_feas_all);
    bool next(vector<int> *pisition, vector<int>*max ,int size);
    void split( int max_split, int max_size_pipe);
    int get_index_of_proc(vector <Procs_Allocation> v,Processor p);
    vector<Stage_Set>  give_task_order(vector<Stage_Set>  sets_of_tasks);
    void create_final_file();
bool check_feasibility_greedy( vector<Processor> *pro,int max_split);



public:
    /**
       Default constructor: num_proc_try is equal to 1;
    */
    CheckFeasibility();
    /**
      Constructor:
      @param orderTAsk : assigned to order_task that is type of algorithm used to select task.
      @param Alg : assigned to proc_algorithm ; It's type of algorithm used to select processor.
      @param tss: assigned to task_sets. Each task_Set contains tasks of one pipe.
      @param period_deadline: used to inizialized Allocation result.
      @param ps: assigned to processors.
        */
    CheckFeasibility( int orderTask, int Alg,  vector<  TaskSet> tss,vector< pair<double,double> >period_deadline, vector<Processor> ps,int v);
    //  void initTaskSelectionSet();

    void check();



};

}
#endif // CHEACKFEASIBILITY_HPP_INCLUDED

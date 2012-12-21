#ifndef CHEACKFEASIBILITY_HPP_INCLUDED
#define CHEACKFEASIBILITY_HPP_INCLUDED


#include <map>
#include <algorithm>
#include <models/task.hpp>
#include <models/Processor.hpp>
#include <models/taskset.hpp>
#include <models/Stage.hpp>
#include<models/Proc_Priority_Comunication.hpp>
#include <models/Proc_Allocation.hpp>
#include <common/UsefulF.hpp>
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
    /** contains all feasible combination of allocation tasks;
        after the execution of check_feasibility_greedy the
        all_feasible_combinations[0] contains the solution
        founded from greedy algorithm
    **/
    vector <Feasible_Allcoation> all_feasible_combinations;
    /** vector contains priority between processors;
        at position i it's contained priority assigned to all
        processors by processor having id i
    **/
    vector<Proc_Priority_Comunication> proc_comunication_priority_data;
    /** vector contains the pipelines that are allocated succesfully;
        pipelines positions are added during simulation to the vector;
        this vector is used in order to performe deallocationt
    **/
    vector<int> index_allocated_pipe;
    /** vector contains the pipelines that din't allocated succesfully;
        pipelines positions are added during simulation to the vector;
        this vector is used in order to performe deallocationt
    **/
    vector<int>index_not_allocated_pipe;
    vector< pair<double,double> >period_deadline;
    int verbose;
    bool deallocation_reallocation;
    /** type of algorithm used to select tasks **/
    orderTaskSelection order_task;
     /** type of algorithm used to select processor **/
    precSelectionAlg proc_algorithm;
    /** processors of simulation system **/
    vector<Processor> processors;
    vector <TaskSet> task_sets;
    ofstream output_file;
    void print_feasible_combination();
    /** select processor for the allocation using policy FF BF WF**/
    int select_Processor(vector<Processor> *proc,  double sigma);

    double compute_partialBW(vector<Stage_Set>sets);
    vector<Processor> give_neighbour_processors(vector<Processor> pr,int level);
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
    /** optimal allocation algorithm **/
    bool check_feasibility_combinations( vector<Processor> *pro,int index_pipe,int max_split,vector<Procs_Allocation  >one_feas_all);
    bool next(vector<int> *pisition, vector<int>*max ,int size);
    /** create all possible partition useful **/
    void split( int max_split, int max_size_pipe);
    /** return positions, in just_allocated vector, of item contains all
        tasks allocated on processor p
    **/
    int get_index_of_proc(vector <Procs_Allocation> just_allocated,Processor p);
    /** return same Stage_Set orderd in increasing way for wcet tasks **/
    vector<Stage_Set>  give_task_order(vector<Stage_Set>  *sets_of_tasks);
    void create_final_file();
    /** greedy allocation algorithm **/
    bool check_feasibility_greedy( vector<Processor> *pro,int max_split);
    /** print result of greedy simulation on file**/
    void create_final_file_greedy(string file_name);
    bool check_one_pipe(int index_pipe,int max_split, vector<Processor>*pro,vector <Procs_Allocation>* one_feas_all_final ,ofstream &u_file);
    void remove_pipe(int index_pipe);

    void recombine_allocation(int max_split, vector<Processor>*proc,ofstream &u_file);
   bool find_processor_using_priority(int level,double deadline,int index_task_set,vector<Stage_Set>sets_of_tasks,double first, vector<Processor> *temp, vector <Procs_Allocation> *one_feas_all,int index_pipe,vector<int>path_level );




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
    CheckFeasibility( int orderTask, int Alg,  vector<  TaskSet> tss,vector< pair<double,double> >period_deadline, vector<Processor> ps,int v, vector<Proc_Priority_Comunication>pr);
    void check();
    void recombine_allocation();
    inline void set_realloc_flag(bool flag){ deallocation_reallocation=flag;}


};


}
#endif // CHEACKFEASIBILITY_HPP_INCLUDED

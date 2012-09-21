#include <CheckFeasibility.hpp>

using namespace std;
using namespace Scan;
namespace Scan
{


int rand_lim(int limit)
{

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do
    {
        retval = rand() / divisor;
    }
    while (retval > limit);

    return retval;
}

// Return a random number between lower and upper inclusive.
int rand_lim(int lower, int upper)
{
    int range = abs(upper-lower);

    return rand_lim(range) + lower;
}

CheckFeasibility::CheckFeasibility():num_proc_try(1), verbose(0)
{

}

CheckFeasibility::CheckFeasibility( int orderTask, int Alg,  vector<  TaskSet> tss,vector< pair<double,double> >period_deadline,vector<Processor> ps,int v)
    :num_proc_available(ps.size()), task_sets(tss),num_proc_try(1), processors(ps),verbose(v)
{

    result=new Allocation(period_deadline);
    switch ( orderTask )
    {
    case 0:
        order_task=orderTaskSelection::precedence;
        break;

    case 2:
        order_task=orderTaskSelection::increase_wcet;
        break;
    case 1:
        order_task=orderTaskSelection::decrease_wcet;
        break;

    default :
        order_task=orderTaskSelection::precedence;
    }


    switch ( Alg )
    {
    case 0:
    {
        proc_algorithm=precSelectionAlg::firstFit;
    }
    break;
    case 1:
    {
        proc_algorithm=precSelectionAlg::bestFit;

    }
    break;
    case 2:
    {
        proc_algorithm=precSelectionAlg::worstFit;
    }

    break;
    default :
    {
        proc_algorithm=precSelectionAlg::bestFit;
    }

    }
    initTaskSelectionSet();
    initProcessor();
}

void CheckFeasibility::initProcessor()
{
    processor_utilised.clear();
    switch (proc_algorithm )
    {
    case firstFit:
    {
        for(int i=0; i<num_proc_try; i++)
        {
            processor_utilised.push_back(processors.at(i));
        }
    }
    break;
    case bestFit:
    {
        sort_proc<ProcCmpUtilizeIncr>();
        for(int i=0; i< num_proc_try; i++)
        {
            processor_utilised.push_back(processors.at(i));

        }
    }
    break;
    case worstFit:
    {
        sort_proc<ProcCmpUtilizeDecr>();
        for(int i=0; i<num_proc_try; i++)
        {
            processor_utilised.push_back(processors.at(i));
        }
    }

    break;
    default :
    {
        sort_proc<ProcCmpUtilizeIncr>();
        for(int i=0; i<num_proc_try; i++)
        {
            processor_utilised.push_back(processors.at(i));
        }

    }

    }
}

void CheckFeasibility::initTaskSelectionSet()
{
    tasks_for_selection.clear();
    switch(order_task)
    {
    case increase_wcet:
    {
        for(int i=0; i<task_sets.size(); i++)
        {
            task_sets[i].sort<TaskCmpWcet>();
            record *r= new record(task_sets[i][0],i,0);
            tasks_for_selection.push_back(*r);
        }
    }
    break;

    case decrease_wcet:
    {
        for(int i=0; i<task_sets.size(); i++)
        {
            int last_index =task_sets[i].size()-1;
            task_sets[i].sort<TaskCmpWcet>();
            record r= record(task_sets[i][last_index],i,last_index);
            tasks_for_selection.push_back(r);
        }
    }
    break;

    case precedence:
    {
        for(int i=0; i<task_sets.size(); i++)
        {
            record r= record(task_sets[i][0],i,0);
            tasks_for_selection.push_back(r);
        }
    }
    break;
    }
}

void CheckFeasibility::clean_processors()
{
    for(int i= 0; i< result->get_minNumProc(); i++)
    {
        processor_utilised.at(i).clean_data();
    }
}


Allocation& CheckFeasibility:: check()
{
    bool allocation=false;
    bool proc_not_found=false;
    record t=record();

    t=selectTask();
    if(verbose==1)
    {
        cout<< "RECORD SELECTED IS :"<<endl;
        cout<<t;
    }
    while(t.get_Index_In_Pipe()!=-1 && allocation !=true && proc_not_found==false)
    {

        int indexPipe=t.get_Index_Pipe();
        // posizione nella pipe finale allocata //
        Task task=t.get_Task();
        int positionTask=result->get_transaction(indexPipe).get_num_tasks();
        int num_proc_tryBefore= num_proc_try;
        int proc= selectProcessor(indexPipe,positionTask,task );

        if(verbose==1)
            cout <<"PROCESSOR SELECTED IS :"<<proc<<endl;

        if(proc==-1)
        {
            cout << "Processor Not Found"<<endl;
            proc_not_found=true;
            break;
        }

        else
        {
            if (num_proc_tryBefore< num_proc_try)
            {
                return check();
            }
            if(verbose==1)
                cout<<"TASK IS ALLOCATED"<<endl;
            result->allocTask(task,  proc,indexPipe);

        }

        t=selectTask();
        if(verbose==1)
        {
            cout<< "RECORD SELECTED IS :"<<endl;
            cout<<t;
        }
        if(t.get_Index_In_Pipe()==-1)
        {
            cout<< "ALLOCATION IS FOUND"<<endl;
            allocation=true;
            break;

        }
    }
    if(allocation== true)
    {
        result->setMinProc(num_proc_try);
        result->setFeasible(true);
        return *result;
    }

    return *result;
}



double CheckFeasibility::computeBandwidth(int indexPipe,int procSelected, double dead_line, double wcet)
{
    double bound=0;
    double bandwidth=0;
    Transaction t= result->get_transaction(indexPipe);
    double utilisation=processor_utilised.at(procSelected).get_Utilisation()+(wcet/t.get_period());
    for(int i=0; i< t.get_num_tasks(); i++)
    {
        int idProc= t.get_task_node(i);
        for(int j=0; j<processor_utilised.size(); j++)
        {
            int temp_id=processor_utilised.at(j).get_Id();
            if(idProc==temp_id)
            {


                double sigma=processor_utilised.at(j).compute_Time(indexPipe,i);

                if(processor_utilised.at(procSelected).get_Id()==temp_id)
                    bound=bound+(sigma/utilisation);
                else
                {
                    double util=processor_utilised.at(j).get_Utilisation();
                    bound= bound+(sigma/util);
                }

            }
        }

    }
    double ct=processor_utilised.at(procSelected).get_Computation_Time();
    double numeratore=((ct+wcet) /utilisation);
    numeratore= numeratore+bound;
    bound= numeratore/dead_line;
    bandwidth=bound* utilisation;
    return bandwidth;
}

int CheckFeasibility::selectProcessor(int indexPipe, int positionTask,Task &t)
{
    double deadline=t.get_dline();
    double computationTime= t.get_wcet();
    switch ( proc_algorithm)
    {
    case firstFit:
    {


        for(int i=0; i< processor_utilised.size(); i++)
        {

            double bandwidth= computeBandwidth(indexPipe, i, deadline,computationTime);
            if(verbose==1)
            cout<< "bandwidth:"<<bandwidth<<endl;
            double free=processor_utilised.at(i).get_free();
            if(free>bandwidth)
            {
                processor_utilised.at(i).update_Processor(indexPipe, positionTask,t,bandwidth);
                return processor_utilised.at(i).get_Id();
            }

        }
    }
    break;
    case bestFit:
    {
        sort_proc<ProcCmpUtilizeIncr>();



        for(int i=0; i< processor_utilised.size(); i++)
        {

            double bandwidth= computeBandwidth(indexPipe, i, deadline,computationTime);
            if(verbose==1)
            cout<< "bandwidth:"<<bandwidth<<endl;
            double free=processor_utilised.at(i).get_free();
            if(free>bandwidth)
            {
                processor_utilised.at(i).update_Processor(indexPipe, positionTask,t,bandwidth);
                return processor_utilised.at(i).get_Id();
            }
        }
    }
    break;
    case worstFit:
    {
        sort_proc<ProcCmpUtilizeDecr>();


        for(int i=0; i< processor_utilised.size(); i++)
        {
            double bandwidth= computeBandwidth(indexPipe, i, deadline,computationTime);
             if(verbose==1)
            cout<< "bandwidth:"<<bandwidth<<endl;
            double free=processor_utilised[i].get_free();
            if(free>bandwidth)
            {
                processor_utilised.at(i).update_Processor(indexPipe, positionTask,t,bandwidth);
                return  processor_utilised.at(i).get_Id();
            }
        }
    }
    break;
    }
    if( num_proc_try >=num_proc_available)
        return -1;
    else
    {

        num_proc_try++;
        if(verbose==1)
        cout<<"I'm going to increment number of processors used : "<<num_proc_try<<endl;
        clean_processors();
        initTaskSelectionSet();
        initProcessor();
        result->clear();
        return 1;
        //selectProcessor( indexPipe,  positionTask,t);

    }
}






record & CheckFeasibility:: selectTask()
{
    if(tasks_for_selection.size()==0)
    {
        record * r = new record();
        return *r;
    }


    record *returned =  new record();
    TaskCmpWcet obj= TaskCmpWcet();
    switch ( order_task )
    {
    case precedence:
    {
        int index= rand_lim(0,tasks_for_selection.size()-1);
        *returned=tasks_for_selection[index];
        int indexPipe=returned->get_Index_Pipe();
        int j= returned->get_Index_In_Pipe();
        j++;
        if(j<task_sets[indexPipe].size() )
        {
            record  r= record(task_sets[indexPipe][j],indexPipe,j) ;
            tasks_for_selection.insert(tasks_for_selection.begin()+ indexPipe, r);
            tasks_for_selection.erase (tasks_for_selection.begin()+indexPipe+1);

            //tasks_for_selection[indexPipe]=r;
        }
        else
        {
            tasks_for_selection.erase (tasks_for_selection.begin()+index);
        }
        return *returned;
    }

    break;
    case increase_wcet:
    {
        Task minT= tasks_for_selection[0].get_Task();
        int index_min=0;
        *returned=tasks_for_selection[0];
        for(int i=1; i<tasks_for_selection.size(); i++)
        {
            if(!(obj)(minT,tasks_for_selection[i].get_Task()))
            {

                *returned=tasks_for_selection[i];
                minT= tasks_for_selection[i].get_Task();
                index_min=i;
            }

        }
        // returned->printRecord();
        int i=returned->get_Index_Pipe();
        int j=returned->get_Index_In_Pipe()+1;
        if(task_sets[i].size()>j)
        {
            record  r= record(task_sets[i][j],i,j) ;
            tasks_for_selection.insert(tasks_for_selection.begin() + index_min, r);
            tasks_for_selection.erase (tasks_for_selection.begin()+index_min+1);

            // tasks_for_selection[index_min]=r;
        }
        else
        {
            tasks_for_selection.erase (tasks_for_selection.begin()+index_min);
        }
        return *returned;

    }
    break;
    case decrease_wcet:
    {
        Task max= tasks_for_selection[0].get_Task();
        *returned= tasks_for_selection[0];
        int index_max=0;
        for(int i=1; i<tasks_for_selection.size(); i++)
        {
            if((obj)(max,tasks_for_selection[i].get_Task()))
            {
                *returned=tasks_for_selection[i];
                max= tasks_for_selection[i].get_Task();
                index_max=i;
            }

        }
        int i=returned->get_Index_Pipe();
        int j=returned->get_Index_In_Pipe()-1;
        if(j>=0)
        {
            record r= record(task_sets[i][j],i,j) ;
            tasks_for_selection.insert(tasks_for_selection.begin() + index_max, r);
            tasks_for_selection.erase (tasks_for_selection.begin()+index_max+1);

        }
        else
        {
            tasks_for_selection.erase (tasks_for_selection.begin()+index_max);
        }

        break;
    }

    default :
        order_task=orderTaskSelection::precedence;
    }
    return *returned;
}

void CheckFeasibility::printProcessor()
{
    for(int i=0; i <processor_utilised.size(); i++)
        cout <<processor_utilised.at(i);
}


};





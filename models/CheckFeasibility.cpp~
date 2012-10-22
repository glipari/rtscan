#include <models/CheckFeasibility.hpp>

using namespace std;
using namespace Scan;
namespace Scan
{

CheckFeasibility::CheckFeasibility(): verbose(0)
{

}

CheckFeasibility::CheckFeasibility( int orderTask, int Alg,  vector<  TaskSet> tss,vector< pair<double,double> >p_d,vector<Processor> ps,int v)
    :task_sets(tss), processors(ps),verbose(v)
{
    output_file.open("Output.txt");
    for(int j=0; j<p_d.size(); j++)
    {
        period_deadline.push_back(p_d.at(j));
    }

    printProcessor(processors);
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
}


void CheckFeasibility::print_feasible_combination()
{
    cout<<endl;
    cout<<endl;
    output_file<<endl;
    output_file<<endl;
    cout<<endl;
    cout<<endl;
    output_file<<"FINAL RESULT:::"<<endl;
    output_file<<endl;
    output_file<<endl;
    cout<<"---------- ------>>>>>>>>>>>  RISULTATO FINALE :: -------------"<<endl;
    output_file<<endl;
    cout<<endl;
    cout<<endl;
    output_file<<endl;
    cout<<"size allocation :"<<feasible_all.size()<<endl;
    for(int i=0; i < feasible_all.size(); i++)
    {
        cout <<feasible_all.at(i);
        output_file<<feasible_all.at(i);
    }
}

void CheckFeasibility::create_final_file()
{
    vector<int> number_task_division;
    int num_pipe_allocated=0;
    ofstream output_file_task;
    ofstream output_file_proc;
    output_file_task.open("ResultT.txt");
    output_file_proc.open("ResultP.txt");
    for(int i=0; i<feasible_all.size(); i++)
    {
        for(int r=0; r<task_sets.size(); r++)
            number_task_division.push_back(0);

        for(int t=0; t<feasible_all.at(i).size(); t++)
        {
            Procs_Allocation pa=feasible_all.at(i).get_at(t);
            for(int s=0; s< pa.size(); s++)
            {
                Stage_Set ss=pa.get_at(s);
                if(ss.size()>0)
                {
                    number_task_division[s]++;

                }
            }

        }


        for(int t=0; t<number_task_division.size(); t++)
        {
            if(number_task_division[t]>0)
            {

                num_pipe_allocated++;
                output_file_task<<number_task_division.at(t);
                output_file_task<<"  ";
            }
            number_task_division[t]=0;


        }
        output_file_task<<endl;
        output_file_task<<"pipe allocate : ";
        output_file_task<<num_pipe_allocated;
        num_pipe_allocated=0;
        output_file_task<<endl;
        output_file_proc<<feasible_all.at(i).size();
        output_file_proc<<endl;
        for( int h=0; h<feasible_all.at(i).size(); h++)
        {
            output_file_proc<<feasible_all.at(i).get_at(h).get_Processor().get_utilised_Bw();
            output_file_proc<<" ";
            output_file_proc<<endl;
        }
        output_file_proc<<endl;
        output_file_proc<<endl;
    }
    output_file_task.close();
    output_file_proc.close();
}


void CheckFeasibility::check()
{
    int max_split;
    /** compute the maximum number of
    subsets in which tasks of each pipes can be
    grouped and asigned to processor.
    this value is minimum value between
    number of processor and max size of pipes **/
    int max_size= task_sets.at(0).size();
    for(int h=1; h<task_sets.size(); h++)
    {
        if(max_size< task_sets.at(h).size())
            max_size=task_sets.at(h).size();
    }
    max_split =max_size;
    if(max_size> processors.size())
        max_split=processors.size();
    /** we obtein all possible combinations
    express by arrays of intege */
    split(max_split,max_size);
    /**for(int i=0; i<task_sets.size(); i++)
    {**/
    if(verbose==1)
        cout<< "CHECK pipe di indice "<<0 <<endl;
    vector <Processor > proc;
    /** copy of processors **/
    for(int j=0; j<processors.size(); j++)
    {
        proc.push_back(processors.at(j));
    }
    /** split(max_split,max_size);**/
    vector<Procs_Allocation>first_feasible;
    check_feasibility_combinations(&proc, 0,max_split,first_feasible);
   //check_feasibility_greedy(&processors,max_split);
    create_final_file();
    //print_feasible_combination();
    output_file.close();
}


int CheckFeasibility::select_Processor(vector<Processor> *proc, double bw)
{
    double free=0;
    bool used=false;
    switch(proc_algorithm)
    {
    case firstFit:
    {
        for(int i=0; i <proc->size(); i++)
        {
            free=proc->at(i).get_free();
            used=proc->at(i).get_flag_utilised();
            if(bw<=free)
            {
                if(verbose==1)
                    cout<<"bw :"<<bw<<" < "<< "free : "<<free <<endl;
                if(used==false)
                {
                    if(verbose==1)
                        cout <<" si ritorno il processore : " <<i<<endl;
                    proc->at(i).update(bw);
                    return i;
                }
            }
        }
    }
    break;
    case bestFit:
    {

        sort(proc->begin(),proc->end(), ProcCmpUtilizeIncr());
//          for(int l=0;l<proc->size();l++)
//                cout<<proc->at(l);
        for(int i=0; i <proc->size(); i++)
        {
            free=proc->at(i).get_free();
            used=proc->at(i).get_flag_utilised();
            if(bw<=free)
            {
                if(verbose==1)
                    cout<<"bw :"<<bw<<" < "<< "free : "<<free <<endl;

                if(used==false)
                {
                    if(verbose==1)
                        cout <<" si ritorno il processore : " <<i<<endl;
                    proc->at(i).update(bw);
                    return  i;
                }

            }
        }
    }
    break;
    case worstFit:
    {
        sort(proc->begin(),proc->end(), ProcCmpUtilizeDecr());

        for(int i=0; i <proc->size(); i++)
        {
            free=proc->at(i).get_free();
            used=proc->at(i).get_flag_utilised();
            if(bw<=free)
            {
                if(verbose==1)
                    cout<<"bw :"<<bw<<" < "<< "free : "<<free <<endl;

                if(used==false)
                {
                    if(verbose==1)
                        cout <<" si ritorno il processore : " <<i<<endl;
                    proc->at(i).update(bw);
                    return i;
                }

            }
        }
    }
    break;
    }
    return -1;
}


void CheckFeasibility::printProcessor(vector< Processor>p)
{
    for(int i=0; i <p.size(); i++)
        cout <<p.at(i);
}


void CheckFeasibility::split( int max_split, int max_size_pipe)
{
    vector<int >p;
    vector <int> m;
    vector<int >* position=&p;
    vector <int> *max=&m;
    for(int j=0; j<max_size_pipe; j++)
    {
        position->push_back(1);
        max->push_back(1);
    }
    positions_combinations.push_back(*position);

    while(next(position,max,max_split ))
        {}
    /** stampa **/
    if(verbose==1)
    {
        for(int k=0; k< positions_combinations.size(); k++)
        {
            for(int h=0; h< positions_combinations.at(k).size(); h++)
                cout<<positions_combinations.at(k).at(h);
            cout<<endl;
            cout<<"----"<<endl;
        }
    }
}


template <class t>
void stampa_vettore(vector<t>v)
{
    for(int i=0; i<v.size(); i++)
        cout<<v.at(i);
    cout<<endl;
}

bool CheckFeasibility::next(vector<int> *position, vector<int>*max ,int size)
{
    int i=0;
    ++(position->at(i));
    bool not_insert= false;
    while(i< position->size()-1 && (position->at(i) > max->at(i+1)+1))
    {
        (*position)[i]=1;
        i++;
        ++position->at(i);

    }
    for(int h=0; h< position->size()&& not_insert==false; h++)
    {
        if(position->at(h)>size)
        {
            not_insert=true;
        }
    }
    if(i==position->size()-1)
    {
        return false;
    }
    if(position->at(i) > max->at(i))
    {
        (*max)[i]=(*position)[i];
    }
    for(int j=i-1; j>=0; j--)
    {
        (*max)[j]=(*max)[i];
    }
    if(position->at(0)<= size && not_insert==false )
    {
        positions_combinations.push_back(*position);
    }
    return true;
}

bool CheckFeasibility::is_last_combination(int index, int size, int max_split)
{
    int s= 1;
    /** check if combination at index "index" is last
    combinatio that have the structure increased, so
    for a pipe of 3 stage the last combination is 1 2 3:**/
    if(size==1)
        if(index==0)
            return true;
    for(int i= size-1; i>=0 ; i--)
    {
        if(positions_combinations.at(index).at(i)!=s)
            return false;
        s++;
    }
    for(int j=size-max_split-1; j>0; j++)
    {
        if( positions_combinations.at(index).at(j)!=max_split)
            return false;
    }
    return true;
}

vector<int> CheckFeasibility::get_combination(int size_pipe,int index)
{
    vector <int> temp;
    for(int i =0; i <size_pipe; i++)
    {
        temp.push_back(positions_combinations.at(index).at(i));
        if(verbose==1)
            cout<<temp.at(i);
    }
    cout<<endl;
    return temp;
}


vector <Stage_Set> CheckFeasibility::get_Stage_Set(vector <int> position, int index_pipe)
{
    vector <Stage_Set> rss;
    for(int i=0; i< position.size(); i++)
    {
        if(rss.size() < position.at(i))
        {
            for(int j=rss.size(); j<position.at(i); j++)
            {
                Stage_Set *ts= new Stage_Set();
                rss.push_back(*ts);
            }
        }
        Task t=task_sets.at(index_pipe).at(i);
        Stage *r= new Stage(t,index_pipe,i);
        rss.at(position.at(i)-1)+=(*r);
    }
    if(verbose==1)
    {
        cout<<"Stage set che formano la combinazione :"<<rss.size()<<endl;
        for(int k=0; k<rss.size(); k++)
        {
            cout<<rss.at(k);
        }
    }
    return rss;
}


double CheckFeasibility::compute_partialBW(vector<Stage_Set>sets)
{
    double tot=0;
    vector <Stage_Set>t;
    for(int i=0; i < sets.size(); i++)
    {
        tot=tot+ (sets.at(i).get_bw_param()/sets.at(i).get_util());
    }
    return tot;
}

int CheckFeasibility::get_index_of_proc(vector <Procs_Allocation> v,Processor p)
{

    for(int k=0; k< v.size(); k++)
    {
        if(v.at(k).is_processor(p.get_Id()))
            return k;

    }
    return -1;
}

vector<Stage_Set> CheckFeasibility:: give_task_order(vector<Stage_Set>  sets_of_tasks)
{
    switch ( order_task )
    {
    case increase_wcet:
    {
        sort_Stage_Sets<CmpWcetIncr>( sets_of_tasks) ;
    }
    break;
    case decrease_wcet:
    {
        sort_Stage_Sets<CmpWcetDecr>( sets_of_tasks);
    }
    }
    return sets_of_tasks;
}

vector <Processor> create_procs_copy(vector<Processor> ps)
{
    vector<Processor>proc;
    for(int h=0; h<ps.size(); h++)
    {
        proc.push_back(ps.at(h));
        proc.at(proc.size()-1).set_flag_utilised(false);
    }
    return proc;
}

bool CheckFeasibility::check_feasibility_combinations( vector<Processor> *pro,int index_pipe,int max_split,vector<Procs_Allocation  >feasible_other_pipe)
{
    double sigma=0;
    double bw=0;
    double first= true;
    bool feasible=false;
    vector <Processor > proc;
    vector<Procs_Allocation>::iterator it;
    int stage_number= task_sets.at(index_pipe).size();
    int i=0;
    vector <int> combination=get_combination(stage_number, i);
    bool is_last=false;
    double deadline=period_deadline.at(index_pipe).second;
    while(is_last!=true|| first==true)
    {
        is_last=is_last_combination(i,stage_number,max_split);
        proc=create_procs_copy(*pro);
        vector<Stage_Set>sets_of_tasks= give_task_order(get_Stage_Set(combination,index_pipe));
        double sigma= compute_partialBW(sets_of_tasks)/deadline;
        int index_proc=-1;
        feasible=true;
        vector <Procs_Allocation> one_feas_all;
        it= one_feas_all.begin();
        one_feas_all.insert(it, feasible_other_pipe.begin(),feasible_other_pipe.end());
        bool exit=false;
        for(int k=0; k<sets_of_tasks.size()&&  feasible==true && exit==false; k++)
        {
            if(first==true)
            {
                bw=sets_of_tasks.at(0).get_util();
                k=0;
                exit=true;
            }
            else
                bw=sets_of_tasks.at(k).get_util()*sigma;
            index_proc=select_Processor(&proc,bw);

            if(index_proc!=-1)
            {
                int index=get_index_of_proc(one_feas_all,proc.at(index_proc));
                if(index!=-1)
                {
                    one_feas_all.at(index).update_task_allocated(proc.at(index_proc),index_pipe, sets_of_tasks.at(k));
                }
                else
                {
                    Procs_Allocation *one_feas= new Procs_Allocation(proc.at(index_proc),index_pipe, sets_of_tasks.at(k));
                    one_feas_all.push_back(*one_feas);
                }
            }
            else
                feasible= false;
            first=false;
        }
        if(feasible==true)
        {
            if(index_pipe== task_sets.size()-1)
            {
                Feasible_Allcoation *pipes_allocated= new Feasible_Allcoation(one_feas_all);
                feasible_all.push_back(*pipes_allocated);

            }
            else
                check_feasibility_combinations(&proc, index_pipe +1, max_split, one_feas_all);
        }
        proc.clear();
        i++;
        if(i <positions_combinations.size())
            combination=get_combination(stage_number, i);
        else
            break;
    }
    if(feasible==false &&index_pipe<task_sets.size()-1)
        check_feasibility_combinations(pro, index_pipe +1, max_split, feasible_other_pipe);
    return true;
}



bool CheckFeasibility::check_feasibility_greedy( vector<Processor> *pro,int max_split)
{
    double sigma=0;
    double bw=0;
    double first= true;
    vector <Processor > proc;
    vector<Procs_Allocation>::iterator it;
    int i=0;
    vector <Procs_Allocation> one_feas_all;
    proc=create_procs_copy(*pro);
    for(int i=0; i<task_sets.size(); i++)
    {
        int stage_number= task_sets.at(i).size();
        int j=0;
        vector <int> combination=get_combination(stage_number, j);
        double deadline=period_deadline.at(i).second;
        vector<Stage_Set>sets_of_tasks= give_task_order(get_Stage_Set(combination,i));
        first=true;
        bool found=false;
        bool is_last=false;
        while(is_last!=true&& found!=true)
        {
            is_last=is_last_combination(j,stage_number,max_split);
            double sigma= compute_partialBW(sets_of_tasks)/deadline;
            int index_proc=-1;
            bool feasible=true;
            int size=sets_of_tasks.size();
            vector<Processor>temp=create_procs_copy(proc);

            for(int k=0; k<size && feasible; k++)
            {
                if(first==true)
                {
                    bw=sets_of_tasks.at(0).get_util();
                    k=0;
                    first=false;
                }
                else
                    bw=sets_of_tasks.at(k).get_util()*sigma;
                index_proc=select_Processor(&temp,bw);
                if(index_proc!=-1)
                {
                    int index=get_index_of_proc(one_feas_all,temp.at(index_proc));

                    if(index!=-1)
                        one_feas_all.at(index).update_task_allocated(temp.at(index_proc),i, sets_of_tasks.at(k));
                    else
                    {
                        Procs_Allocation *one_feas= new Procs_Allocation(temp.at(index_proc),i, sets_of_tasks.at(k));
                        one_feas_all.push_back(*one_feas);
                    }
                }
                else
                {
                    feasible=false;
                    j++;
                    if(j<positions_combinations.size())
                    {
                        combination=get_combination(stage_number, j);
                        sets_of_tasks= give_task_order(get_Stage_Set(combination,i));
                    }

                }
            }
            if(feasible)
            {
                found=true;
                proc=create_procs_copy(temp);
                if(i== task_sets.size()-1)
                {
                    Feasible_Allcoation *pipes_allocated= new Feasible_Allcoation(one_feas_all);
                    feasible_all.push_back(*pipes_allocated);

                }
            }
        }
    }

}


}


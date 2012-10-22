#include <models/CheckFeasibility.hpp>

using namespace std;
using namespace Scan;
namespace Scan
{


CheckFeasibility::CheckFeasibility(): verbose(0)
{

}

CheckFeasibility::CheckFeasibility( int orderTask, int Alg,  vector<  TaskSet> tss,vector< pair<double,double> >p_d,vector<Processor> ps,vector<vector<int>> neigh,int v)
    :task_sets(tss), processors(ps),verbose(v)
{
    output_file.open("Output.txt");
    for(int j=0; j<p_d.size(); j++)
        period_deadline.push_back(p_d.at(j));

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
    cout<<"size allocation :"<<all_feasible_combinations.size()<<endl;
    for(int i=0; i < all_feasible_combinations.size(); i++)
    {
        cout <<all_feasible_combinations.at(i);
        output_file<<all_feasible_combinations.at(i);
    }
}

void CheckFeasibility::create_final_file()
{
    vector<int> number_task_division;
    int index_best_stege=-1;
    int index_best_proc_num=-1;
    int index_best_proc_bw=-1;
    double average_bw=1;
    double num_average_min_division=1000;
    int num_max_pipe_allocated=0;

    int min_num_proc=processors.size()+1;
    ofstream output_file_task;
    output_file_task.open("ResultT.txt");
    for(int i=0; i<all_feasible_combinations.size(); i++)
    {
        int num_pipe_allocated=0;
        int tot_tasks=0;
        int num_proc=all_feasible_combinations.at(i).size();
        for(int r=0; r<task_sets.size(); r++)
        {
            number_task_division.push_back(0);
        }
        double bw=0;

        for(int t=0; t<all_feasible_combinations.at(i).size(); t++)
        {
            Procs_Allocation pa=all_feasible_combinations.at(i).get_at(t);
            bw=bw +pa.get_Processor().get_utilised_Bw();

            for(int s=0; s< pa.size(); s++)
            {
                Stage_Set ss=pa.get_at(s);
                if(ss.size()>0)
                {
                    number_task_division[s]=number_task_division[s]+1;

                }
            }

        }
        for(int t=0; t<number_task_division.size(); t++)
        {
            tot_tasks=tot_tasks +number_task_division[t];
            if(number_task_division[t]>0)
            {
                num_pipe_allocated++;
            }
            number_task_division[t]=0;
        }

        if(num_pipe_allocated>num_max_pipe_allocated)
        {
            index_best_stege=i;
            num_max_pipe_allocated=num_pipe_allocated;
        }
        bw= bw/num_proc;
        if(num_pipe_allocated==num_max_pipe_allocated)
        {
            double av_num_s=(double)tot_tasks/(double)num_pipe_allocated;
            if(av_num_s< num_average_min_division)
            {
                index_best_stege=i;
                num_average_min_division=av_num_s;
            }
            if(num_proc<min_num_proc)
            {
                min_num_proc=num_proc;
                index_best_proc_num=i;
            }
            if(bw<average_bw)
            {
                average_bw=bw;
                index_best_proc_bw=i;
            }
        }
        num_pipe_allocated=0;
    }
    output_file_task<<"PIPE ALLOCATE:";
    output_file_task<<num_max_pipe_allocated;
    output_file_task<<endl;
    output_file_task<<"migliore per banda";
    output_file_task<<endl;
    output_file_task<<average_bw;
    output_file_task<<endl;
    output_file_task<<all_feasible_combinations[index_best_proc_bw];
    output_file_task<<endl;
    output_file_task<<"migliore per num_proc";
    output_file_task<<endl;
    output_file_task<<min_num_proc;
    output_file_task<<endl;
    output_file_task<<all_feasible_combinations[index_best_proc_num];
    output_file_task<<endl;
    output_file_task<<"migliore per numero stage";
    output_file_task<<endl;
    output_file_task<<num_average_min_division;
    output_file_task<<endl;
    output_file_task<<all_feasible_combinations[index_best_stege];
    output_file_task<<endl;
    output_file_task.close();
}


void CheckFeasibility::create_final_file_greedy(string file_name)
{
    vector<int> number_task_division;
    int num_pipe_allocated=0;
    ofstream output_file_task;
    ofstream output_file_proc;
    output_file_task.open(file_name);
    for(int r=0; r<task_sets.size(); r++)
        number_task_division.push_back(0);

    if(all_feasible_combinations.size()!=0)
    {
        for(int t=0; t<all_feasible_combinations.at(0).size(); t++)
        {
            Procs_Allocation pa=all_feasible_combinations.at(0).get_at(t);
            cout<<pa;
            for(int s=0; s< pa.size(); s++)
            {
                Stage_Set ss=pa.get_at(s);
                if(ss.size()>0)
                    number_task_division[s]++;
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
        output_file_task<<all_feasible_combinations.at(0);
    }
    else
        output_file<<"no feasible combination";
    output_file_task<<endl;
    output_file_task.close();
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
    for(int i=0; i<positions_combinations.size(); i++)
        stampa_vettore(positions_combinations.at(i));
    if(verbose==1)
        cout<< "CHECK pipe di indice "<<0 <<endl;
    vector <Processor > proc;
    /** copy of processors **/
    for(int j=0; j<processors.size(); j++)
    {
        proc.push_back(processors.at(j));
        cout<<proc.at(j);
    }
    /** split(max_split,max_size);**/
    vector<Procs_Allocation>first_feasible;

    //check_feasibility_combinations(&proc, 0,max_split,first_feasible);
    //create_final_file();
    //feasible_all.clear();
    check_feasibility_greedy(&processors,max_split);
    output_file.close();
}

vector<Processor> CheckFeasibility::give_neighbour_processors(vector<Processor> pr)
{
    vector<Processor> proc;
    vector <int> index;
    for(int i=0; i<pr.size(); i++)
    {
        if(pr.at(i).get_flag_utilised()==true)
        {

            for(int k=0; k<pr.at(i).get_neighbour().size(); k++)
            {
                std::vector <int>::iterator it;
                int neig=pr.at(i).get_neighbour().at(k);
                it = std::find( index.begin(), index.end(), neig);

                if ( it == index.end() )
                {
                    index.push_back(neig );
                }
            }
        }
    }
    for(int j=0; j<index.size(); j++)
    {
        int h= get_position_in_proc_vector(index.at(j),pr);
        proc.push_back(pr.at(h));
    }
    return proc;

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
                    return proc->at(i).get_Id();
                }
            }
        }
    }
    break;
    case bestFit:
    {

        sort(proc->begin(),proc->end(), ProcCmpUtilizeIncr());
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
                    return  proc->at(i).get_Id();
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
                    return proc->at(i).get_Id();
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
    }
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


bool CheckFeasibility::check_feasibility_combinations( vector<Processor> *pro,int index_pipe,int max_split,vector<Procs_Allocation  >feasible_other_pipe)
{
    double sigma=0;
    double bw=0;
    double first= true;
    bool feasible=false;
    vector <Processor > proc;
    vector<Processor>neighbour;
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
        int id_proc=-1;
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
            neighbour.clear();
            neighbour=give_neighbour_processors(proc);
            if(neighbour.size()!=0)
            {
                id_proc=select_Processor(&neighbour,bw);
                int i_update=get_position_in_proc_vector(id_proc,proc);
                int j_from_update=get_position_in_proc_vector(id_proc,neighbour);
                proc[i_update]=neighbour[j_from_update];
            }

            if(id_proc==-1)
            {
                id_proc=select_Processor(&proc,bw);
            }
            if(id_proc!=-1)
            {

                int index_proc=get_position_in_proc_vector(id_proc,proc);
                int index=get_index_of_proc(one_feas_all,proc.at(index_proc));
                if(index!=-1)
                {
                    one_feas_all.at(index).update_task_allocated(proc.at(index_proc),index_pipe, sets_of_tasks.at(k),bw);
                }
                else
                {
                    Procs_Allocation *one_feas= new Procs_Allocation(proc.at(index_proc),index_pipe, sets_of_tasks.at(k),bw);
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
                all_feasible_combinations.push_back(*pipes_allocated);

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
    ofstream u_file;
    u_file.open("Ufile.txt");
    vector <Processor > proc;
    vector <Procs_Allocation> one_feas_all_final;
    //proc=create_procs_copy(*pro);
    for(int i=0; i<task_sets.size(); i++)
    {

        check_one_pipe(i,max_split,pro,&one_feas_all_final,u_file);
    }
    Feasible_Allcoation *pipes_allocated= new Feasible_Allcoation(one_feas_all_final);
    all_feasible_combinations.push_back(*pipes_allocated);
    create_final_file_greedy("Gready_before.txt");
    if( deallocation_reallocation)
    {
        u_file<<"RECOMBINE START";
        u_file<<endl;
        recombine_allocation(max_split,&proc,u_file);
    }
    create_final_file_greedy("Gready_after.txt");
    u_file.close();
}


void CheckFeasibility::recombine_allocation(int max_split, vector<Processor>*proc,ofstream &u_file)
{
    u_file<<"-----";
    int how_dealloc=rand_lim(0,index_allocated_pipe.size()/2);
    vector<int> index_selectable_pipe;
    vector<int>::iterator it;
    int i=0;
     ofstream re_file;
    re_file.open("reall.txt");
    if(index_allocated_pipe.size()==0 ||index_not_allocated_pipe.size()==0)
    return;
    while( i < how_dealloc)
    {
        int size=index_allocated_pipe.size()-1;
        int ind=rand_lim(0,size);
        int element= index_allocated_pipe.at(ind);
        it= find(index_selectable_pipe.begin(), index_selectable_pipe.end(), element);

       if(it==index_selectable_pipe.end())
       {
            index_selectable_pipe.push_back(element);
            i++;
       }
    }

    cout <<"Vettore delle pipe da deallocare"<<endl;
    cout<<endl;
    stampa_vettore(index_selectable_pipe);
    re_file<<"Pipe rimosse: ";
    re_file<<endl;
    for(int j=0; j<index_selectable_pipe.size();j++)
    {
        re_file<<index_selectable_pipe.at(j);
        re_file<<";";
        remove_pipe(index_selectable_pipe.at(j));
    }
    re_file<<endl;
    vector <Procs_Allocation> one_feas_all_final;
     vector <Procs_Allocation> temp=all_feasible_combinations.at(0).get_vector_allocation();

    for(int h=0; h< temp.size();h++)
    one_feas_all_final.push_back(temp.at(h));

    cout<<"Stampa delle combinazioni fattibili dopo la rimozione"<<endl;
    cout<<endl;
    stampa_vettore(temp);
    all_feasible_combinations.clear();
    int s=index_not_allocated_pipe.size();
    re_file<<"Pipe inserite: ";
    re_file<<endl;
    for(int k=0; k<s;k++)
    {
        int r=index_not_allocated_pipe.at(k);
       if(check_one_pipe(r,max_split,&processors,&one_feas_all_final,u_file))
       {
           re_file<<r;
           re_file<<";";
       }

    }
    Feasible_Allcoation *pipes_allocated= new Feasible_Allcoation(one_feas_all_final);
    all_feasible_combinations.push_back(*pipes_allocated);
    re_file.close();
}


void CheckFeasibility::remove_pipe(int index)
{
    int id;
    int index_processor;
    double bw;
    for(int i=0; i <all_feasible_combinations.at(0).size();i++)
    {
        Procs_Allocation temp= all_feasible_combinations.at(0).get_at(i);
        bw= temp.get_bw_per_pipe(index);
        if(bw>0)
        {
            id= temp.get_Processor().get_Id();
            index_processor=get_position_in_proc_vector(id,processors);
            processors.at(index_processor).subtraction_bw(bw);
            temp.remove_allocation_pipe(index, processors.at(index_processor));
            all_feasible_combinations[0].insert(temp,i);
        }
        cout <<"FEASIBLE 0 "<<endl;
        cout<<endl;

        cout<<all_feasible_combinations[0];


    }
}
bool CheckFeasibility::check_one_pipe(int i,int max_split, vector<Processor>*proc,vector <Procs_Allocation>* one_feas_all_final ,ofstream &u_file)
{
        double tot_u=0;
        int stage_number= task_sets.at(i).size();
        int j=0;
        vector <int> combination=get_combination(stage_number, j);
        double deadline=period_deadline.at(i).second;
        vector<Stage_Set>sets_of_tasks= give_task_order(get_Stage_Set(combination,i));
        bool first=true;
        bool found=false;
        bool is_last=false;
         bool feasible=true;
        int id_proc=-1;
        while(is_last!=true&& found!=true)
        {
            double tot_u_sum=0;
            vector <Procs_Allocation> one_feas_all;
            for(int p=0; p <one_feas_all_final->size(); p++)
            {
                one_feas_all.push_back(one_feas_all_final->at(p));
            }
            vector<Processor>neighbour;
            is_last=is_last_combination(j,stage_number,max_split);
            sets_of_tasks= give_task_order(get_Stage_Set(combination,i));
            double sigma= compute_partialBW(sets_of_tasks)/deadline;
            int index_proc=-1;
            feasible=true;
            int size=sets_of_tasks.size();
            vector<Processor>temp=create_procs_copy(*proc);
            double bw=0;
            for(int k=0; k<size && feasible; k++)
            {
                if(first==true)
                {
                    bw=sets_of_tasks.at(0).get_util();
                    k=0;
                    first=false;
                    tot_u=bw;
                }
                else
                {
                     bw=sets_of_tasks.at(k).get_util()*sigma;
                     tot_u_sum=tot_u_sum+bw;
                }
                neighbour.clear();
                neighbour=give_neighbour_processors(temp);
                id_proc=-1;
                if(neighbour.size()!=0)
                {
                    id_proc=select_Processor(&neighbour,bw);
                    int i_update=get_position_in_proc_vector(id_proc,temp);
                    int j_from_update=get_position_in_proc_vector(id_proc,neighbour);
                    temp[i_update]=neighbour[j_from_update];
                }
                if(id_proc==-1)
                {
                    id_proc=select_Processor(&temp,bw);
                }
                if(id_proc!=-1)
                {
                    int index_proc=get_position_in_proc_vector(id_proc,temp);
                    int index=get_index_of_proc(one_feas_all,temp.at(index_proc));
                    if(index!=-1)
                    {
                        one_feas_all.at(index).update_task_allocated(temp.at(index_proc),i, sets_of_tasks.at(k),bw);
                    }
                    else
                    {
                        Procs_Allocation *one_feas= new Procs_Allocation(temp.at(index_proc),i, sets_of_tasks.at(k),bw);

                        one_feas_all.push_back(*one_feas);
                    }
                    stampa_vettore(one_feas_all);


                }
                else
                {
                    feasible=false;
                    j++;
                    if(j<positions_combinations.size())
                        combination=get_combination(stage_number, j);
                    else
                        is_last=true;
                }
            }
            if(feasible)
            {
                u_file<<tot_u;
                u_file<<";";
                u_file<<tot_u_sum;
                u_file<<endl;
                one_feas_all_final->clear();
                for(int q=0; q <one_feas_all.size(); q++)
                {
                    one_feas_all_final->push_back(one_feas_all.at(q));
                }
                found=true;
                *proc=create_procs_copy(temp);
                index_allocated_pipe.push_back(i);
            }
            else if(is_last)
            index_not_allocated_pipe.push_back(i);
        }

        return feasible;

}

/*
bool CheckFeasibility::check_feasibility_greedy( vector<Processor> *pro,int max_split)
{
    ofstream u_file;
    double sigma=0;
    u_file.open("Ufile.txt");
    double first= true;
    vector <Processor > proc;
    int i=0;
    double tot_u;
    double tot_u_sum;
    vector <Procs_Allocation> one_feas_all_final;
    proc=create_procs_copy(*pro);


    for(int i=0; i<task_sets.size(); i++)
    {

        tot_u=0;
        int stage_number= task_sets.at(i).size();
        int j=0;
        vector <int> combination=get_combination(stage_number, j);
        double deadline=period_deadline.at(i).second;
        vector<Stage_Set>sets_of_tasks= give_task_order(get_Stage_Set(combination,i));
        first=true;
        bool found=false;
        bool is_last=false;
        int id_proc=-1;
        while(is_last!=true&& found!=true)
        {
            tot_u_sum=0;
            vector <Procs_Allocation> one_feas_all;
            for(int p=0; p <one_feas_all_final.size(); p++)
            {
                one_feas_all.push_back(one_feas_all_final.at(p));
            }
            vector<Processor>neighbour;
            is_last=is_last_combination(j,stage_number,max_split);
            sets_of_tasks= give_task_order(get_Stage_Set(combination,i));
            //stampa_vettore(sets_of_tasks);
            double sigma= compute_partialBW(sets_of_tasks)/deadline;
            int index_proc=-1;
            bool feasible=true;
            int size=sets_of_tasks.size();
            vector<Processor>temp=create_procs_copy(proc);
            double bw=0;
            for(int k=0; k<size && feasible; k++)
            {
                if(first==true)
                {
                    bw=sets_of_tasks.at(0).get_util();
                    k=0;
                    first=false;
                    tot_u=bw;
                }
                else
                {
                     bw=sets_of_tasks.at(k).get_util()*sigma;
                     tot_u_sum=tot_u_sum+bw;
                }


                neighbour.clear();
                neighbour=give_neighbour_processors(temp);
                id_proc=-1;
                if(neighbour.size()!=0)
                {
                    id_proc=select_Processor(&neighbour,bw);
                    int i_update=get_position_in_proc_vector(id_proc,temp);
                    int j_from_update=get_position_in_proc_vector(id_proc,neighbour);
                    temp[i_update]=neighbour[j_from_update];
                }
                if(id_proc==-1)
                {
                    //proc=create_procs_copy(*pro);
                    id_proc=select_Processor(&temp,bw);
                }
                if(id_proc!=-1)
                {

                    int index_proc=get_position_in_proc_vector(id_proc,temp);
                    int index=get_index_of_proc(one_feas_all,temp.at(index_proc));
                   // printProcessor(temp);

                    if(index!=-1)
                    {
                        one_feas_all.at(index).update_task_allocated(temp.at(index_proc),i, sets_of_tasks.at(k));
                        cout<<one_feas_all.at(index);
                       // stampa_vettore(one_feas_all);

                    }
                    else
                    {
                        Procs_Allocation *one_feas= new Procs_Allocation(temp.at(index_proc),i, sets_of_tasks.at(k));
                        one_feas_all.push_back(*one_feas);
                        //cout<<*one_feas;
                        //stampa_vettore(one_feas_all);
                    }

                }
                else
                {
                    feasible=false;
                    j++;
                    if(j<positions_combinations.size())
                    {
                        combination=get_combination(stage_number, j);
                       //stampa_vettore(combination);
                    }
                    else
                        is_last=true;

                }
            }
            if(feasible)
            {
                u_file<<tot_u;
                u_file<<";";
                u_file<<tot_u_sum;
                u_file<<endl;
                one_feas_all_final.clear();
                for(int q=0; q <one_feas_all.size(); q++)
                {
                    one_feas_all_final.push_back(one_feas_all.at(q));
                }
                found=true;
                proc=create_procs_copy(temp);
                //if(size>0)
                   // stampa_vettore(one_feas_all);
            }



        }
        if(i== task_sets.size()-1)
        {
            Feasible_Allcoation *pipes_allocated= new Feasible_Allcoation(one_feas_all_final);
            feasible_all.push_back(*pipes_allocated);
           // cout <<"dimensione : "<<feasible_all.size()<<endl;
           // cout<<*pipes_allocated;

        }



    }

u_file.close();
}

*/
}



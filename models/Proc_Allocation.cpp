#include <Proc_Allocation.hpp>

namespace Scan
{

Procs_Allocation::Procs_Allocation()
{

}
/** constructor
    p: processor in which are allocated tasks in r_set;
    index_pipe: tasks r_set are part of pipeline at index_pipe
**/
Procs_Allocation::Procs_Allocation(Processor p, int index_pipe,Stage_Set r_set,double bw)
{
    for(int i =0; i<=index_pipe+1; i++)
    {
        Stage_Set * r = new Stage_Set();
        task_x_pipe.push_back(*r);
    }
    for(int j =0; j<=index_pipe; j++)
        bw_per_pipe.push_back(0);

    task_x_pipe.at(index_pipe)=r_set;
    bw_per_pipe[index_pipe]=bw;
    proc= p;
}

/** copy constructor */
Procs_Allocation::Procs_Allocation(const Procs_Allocation & p)
{
    proc=p.proc;
    for(int i=0; i< p.task_x_pipe.size(); i++)
        task_x_pipe.push_back(p.task_x_pipe.at(i));
    for(int j=0; j< p.bw_per_pipe.size(); j++)
        bw_per_pipe.push_back(p.bw_per_pipe.at(j));
}

/** some tasks (rset) of pipelines at index_pipe are allocated on processor p and they needed
 ** and utilised bw reseource of p.
 **/
void Procs_Allocation::update_task_allocated(Processor p, int index_pipe,Stage_Set r_set, double bw)
{
    for(int i =task_x_pipe.size()-1; i<=index_pipe; i++)
    {
        Stage_Set * r = new Stage_Set();
        task_x_pipe.push_back(*r);
    }
    for(int j =bw_per_pipe.size()-1; j<=index_pipe; j++)
        bw_per_pipe.push_back(0);
    task_x_pipe.at(index_pipe)=r_set;
    bw_per_pipe[index_pipe]=bw;
    proc= p;
}

/** return tasks of pipelines at index **/
Stage_Set Procs_Allocation::get_at(int index)
{
    return task_x_pipe.at(index);
}

/** retunr resourced utilised by pipeline at index_pipe **/
double Procs_Allocation::get_bw_per_pipe(int index_pipe)
{
    if(index_pipe<bw_per_pipe.size())
        return bw_per_pipe.at(index_pipe);
    else return 0;
}

/** remove tasks allocation on processor p of pipeline at index_pipe **/
void Procs_Allocation::remove_allocation_pipe(int index_pipe, Processor p)
{
    bw_per_pipe[index_pipe]=0;
    Stage_Set * s= new Stage_Set();
    task_x_pipe[index_pipe]=* s;
    proc= p;
}

Procs_Allocation& Procs_Allocation::operator=( const Procs_Allocation &p)
{
    proc=p.proc;
    task_x_pipe.clear();
    bw_per_pipe.clear();
    for(int i =0; i< p.task_x_pipe.size(); i++)
        task_x_pipe.push_back(p.task_x_pipe.at(i));

    for(int j =0; j< p.bw_per_pipe.size(); j++)
        bw_per_pipe.push_back(p.bw_per_pipe.at(j));

    return *this;
}

bool Procs_Allocation::is_processor(int id)
{
    int i=proc.get_Id();
    if(id==i)
        return true;
    else
        return false;
}


Feasible_Allcoation::Feasible_Allcoation()
{

}

Feasible_Allcoation::Feasible_Allcoation(vector <Procs_Allocation> list)
{
    for(int i=0; i <list.size(); i++)
    {
        allocation_per_processor.push_back(list.at(i));
    }

}

Feasible_Allcoation::Feasible_Allcoation(const Feasible_Allcoation &f)
{
    for(int i=0; i <f.allocation_per_processor.size(); i++)
        allocation_per_processor.push_back(f.allocation_per_processor.at(i));

}
void Feasible_Allcoation::add(vector<Procs_Allocation>v)
{
    for(int i=0; i<v.size(); i++)
        allocation_per_processor.push_back(v.at(i));
}

vector<Procs_Allocation> Feasible_Allcoation::get_vector_allocation()
{
    return allocation_per_processor;
}

void Feasible_Allcoation::push_back(Procs_Allocation pa)
{
    allocation_per_processor.push_back(pa);
}

Procs_Allocation Feasible_Allcoation::get_at(int index)
{
    return allocation_per_processor.at(index);
}

void Feasible_Allcoation::insert(Procs_Allocation p, int index)
{
    allocation_per_processor[index]=p;
}

std::ostream &operator<<(ostream &s, Stage_Set &r)
{
    for(int i=0; i<r.size(); i++)
    {
        Stage rec=r[i];
        s<<rec;
    }
    s<<endl;
    s<<endl;
    return s;
}

std::ostream &operator<<(ostream &s, Procs_Allocation &p)
{
    Processor pr=p.get_Processor();
    s<<pr.get_Id();
    //s<<endl;
    double d=pr.get_utilised_Bw();
    s<<":";
    double c=pr.get_utilised_Bw();

    s<<c;
    s<<endl;

    s<<pr;
    for(int i=0; i<p.size(); i++)
    {

        Stage_Set rs=p.get_at(i);
        if(rs.size()!=0)
        {
            s<<"TASKS OF PIPE "<<i<<endl;
            s<<rs;
        }

    }
    return s;
}


std::ostream &operator<<(ostream &s, Feasible_Allcoation &fa)
{
//    s<<" NEW COMBINATION :"<<endl;
//    s<<endl;
//    s<<endl;
    for(int i=0; i<fa.size(); i++)
    {
        // s<<"Processor:: "<<endl;
        Procs_Allocation p=fa.get_at(i);

        s<<p;
    }
    s<<endl;

//    s<<"- END COMBINATION -"<<endl;
//    s<<endl;
//    s<<endl;
    return s;
}
}




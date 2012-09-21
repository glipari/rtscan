#include <Proc_Allocation.hpp>

namespace Scan
{

Procs_Allocation::Procs_Allocation()
{

}
/**CAMBIATO PARAMETRI DEL FOR ANCHE IN UPDATE **/
Procs_Allocation::Procs_Allocation(Processor p, int index_pipe,Stage_Set r_set)
{
    for(int i =0; i<=index_pipe+1; i++)
    {
        Stage_Set * r = new Stage_Set();
        task_x_pipe.push_back(*r);
    }
    task_x_pipe.at(index_pipe)=r_set;
    proc= p;
}

Procs_Allocation::Procs_Allocation(const Procs_Allocation & p)
{
    proc=p.proc;
    for(int i=0; i< p.task_x_pipe.size(); i++)
    {
        task_x_pipe.push_back(p.task_x_pipe.at(i));
    }
}

void Procs_Allocation::update_task_allocated(Processor p, int index_pipe,Stage_Set r_set)
{
    for(int i =task_x_pipe.size()-1; i<=index_pipe; i++)
    {
        Stage_Set * r = new Stage_Set();
        task_x_pipe.push_back(*r);
    }
    task_x_pipe.at(index_pipe)=r_set;
    proc= p;
}

Stage_Set Procs_Allocation::get_at(int index)
{
    return task_x_pipe.at(index);
}

bool Procs_Allocation::is_processor(int id)
{
    if(id==proc.get_Id())
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
        feasible_comb.push_back(list.at(i));
    }

}

Feasible_Allcoation::Feasible_Allcoation(const Feasible_Allcoation &f)
{
    for(int i=0; i <f.feasible_comb.size(); i++)
    {
        feasible_comb.push_back(f.feasible_comb.at(i));

    }

}
void Feasible_Allcoation::add(vector<Procs_Allocation>v)
{
    for(int i=0; i<v.size(); i++)
    {
        feasible_comb.push_back(v.at(i));
    }

}
void Feasible_Allcoation::push_back(Procs_Allocation pa)
{
    feasible_comb.push_back(pa);
}
Procs_Allocation Feasible_Allcoation::get_at(int index)
{
    return feasible_comb.at(index);
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
    s<<pr;
    // for(int i=0; i<p.size()-1; i++)
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
    s<<" NEW COMBINATION :"<<endl;
    s<<endl;
    s<<endl;
    for(int i=0; i<fa.size(); i++)
    {
        s<<"Processor:: "<<endl;
        Procs_Allocation p=fa.get_at(i);
        s<<p;
    }
    s<<endl;

    s<<"- END COMBINATION -"<<endl;
    s<<endl;
    s<<endl;
    return s;
}
}
//

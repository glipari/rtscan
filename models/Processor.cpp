#include <models/Processor.hpp>

namespace Scan{

Processor::Processor():starting_bw(0),utilised_Bw(0),free(1),id(increment),used_flag(false),first_task_used(false)
{
    increment++;
}


Processor::Processor(double u): starting_bw(u),utilised_Bw(u),free(1-u),id(increment),used_flag(false),first_task_used(false)/**,init_free(1-u)**/
{
    increment++;
}


Processor::Processor(const Processor &p)
{
    utilised_Bw=p.utilised_Bw;
    starting_bw=p.starting_bw;
    free= p.free;
    id= p.id;
    used_flag=p.used_flag;
    first_task_used=p.first_task_used;
}

double Processor::get_free()const
{
    return free;
}

double Processor::get_starting_Bw()
{
    return starting_bw;

}
void Processor::subtraction_bw(double bw)
{
    utilised_Bw= utilised_Bw-bw;
    free=free+bw;
    if(utilised_Bw<0)
    throw "Error, processor bandwidth is negative!";
}

double Processor::get_utilised_Bw()const
{
    return utilised_Bw;
}
bool Processor::get_flag_utilised() const
{
    return used_flag;
}
bool Processor::get_first_flag() const
{
    return first_task_used;
}
void Processor::set_flag_utilised(bool f)
{
     used_flag= f;
}
bool Processor::set_first_flag(bool f)
{
    first_task_used=f;
}
int Processor::get_Id() const
{
    return id;
}

bool Processor::update(double bw)
{
    utilised_Bw=utilised_Bw+bw;
    free= free-bw;
    used_flag=true;

}


 Processor& Processor::operator=(const Processor &p)
    {
        starting_bw=p.starting_bw;
      utilised_Bw=p.utilised_Bw;
    free= p.free;
    id= p.id;
    used_flag=p.used_flag;
    first_task_used=p.first_task_used;

        return *this;
    }


    std::ostream &operator<<(ostream &s, Processor &p)
{
     s<<"**ID** :"<<p.get_Id()<<endl;
    s<<"UtilisedBW :"<<p.get_utilised_Bw()<<endl;
    s<<"free bw :"<<p.get_free()<<endl;
    s<<"neighbour"<<endl;
    if(p.get_flag_utilised())
    s<<"is used"<<endl;
    else
    s<<"is not used"<<endl;
    s << "-------------------" << endl;
    return s;
}


}


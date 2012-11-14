#include <models/Processor.hpp>

namespace Scan{

Processor::Processor():utilised_Bw(0),free(1),id(increment),used_flag(false)
{
    increment++;
}


Processor::Processor(double u): utilised_Bw(u),free(1-u),id(increment),used_flag(false)/**,init_free(1-u)**/
{
    increment++;
}


Processor::Processor(const Processor &p)
{
    utilised_Bw=p.utilised_Bw;
    free= p.free;
    id= p.id;
    used_flag=p.used_flag;
}

double Processor::get_free()const
{
    return free;
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
void Processor::set_flag_utilised(bool f)
{
     used_flag= f;
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
      utilised_Bw=p.utilised_Bw;
    free= p.free;
    id= p.id;
    used_flag=p.used_flag;

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


#include <string>

#include <models/Stage_Set.hpp>

namespace Scan
{

using namespace std;

Stage_Set::Stage_Set() : s()
{
}

Stage_Set::~Stage_Set()
{
}

Stage_Set::Stage_Set(const Stage_Set &ts)
{
    for(int i=0; i<ts.size(); i++)
    {
        s.push_back(ts.s.at(i));
    }

}

Stage_Set& Stage_Set::operator=(const Stage_Set &ts)
{
    for(int i=0; i<ts.size(); i++)
    {
        s.push_back(ts.s.at(i));
    }

    return *this;
}
double  Stage_Set::get_tot_wcets()const
{
    double tot=0;
    for(int i=0; i<s.size(); i++)
    {
        tot=tot+ s.at(i).get_Task().get_wcet();
    }
    return tot;
}

const Stage&  Stage_Set::at(unsigned int i) const throw(IndexOutOfBound)
{
    if (i >= size())
        throw IndexOutOfBound("Index out of bound in accessing task set");
    return s[i];
}
Stage_Set& Stage_Set::operator+=(const Stage &t)
{
    s.push_back(t);
    return *this;
}

Stage_Set& Stage_Set::operator+=(const Stage_Set &ts)
{
    for (unsigned int i=0; i < ts.size(); ++i)
    {
        s.push_back(ts.at(i));
    }
    return *this;
}

Stage& Stage_Set::operator[](unsigned int i) throw (IndexOutOfBound)
{
    if (i >= size())
        throw IndexOutOfBound("Index out of bound in accessing task set");
    return s[i];
}



Stage_Set operator+(const Stage_Set &ts1, const Stage_Set &ts2)
{
    Stage_Set tset(ts1);
    tset += ts2;
    return tset;
}


std::vector<int> Stage_Set::get_periods() const
{
    unsigned int i;
    std::vector<int> v;
    for (i=0; i<size(); ++i)
        v.push_back(s[i].get_Task().get_period());

    return v;
}

std::vector<double> Stage_Set::get_dlines() const
{
    unsigned int i;
    std::vector<double> v;
    for (i=0; i<size(); ++i)
        v.push_back(s[i].get_Task().get_dline());

    return v;
}
std::vector<double> Stage_Set::get_wcets() const
{
    unsigned int i;
    std::vector<double> v;
    for (i=0; i<size(); ++i)
        v.push_back(s[i].get_Task().get_wcet());

    return v;
}

double Stage_Set::get_bw_param()
{
    vector<double>wcets= get_wcets();
    double tot_wcet=0;
    double wcet=0;
    vector<double>wcets_less_consecutive;
    for(int i=0; i<wcets.size(); i++)
    {
        wcet= wcets.at(i);
        if(i!=wcets.size()-1)
        {
            Stage r= s[i];
            int index_in_pipe= r.get_Index_In_Pipe();
            int j= i+1;
            Stage succesive=s[j];
            int index_in_p= succesive.get_Index_In_Pipe();
            while((i<wcets.size()-1) && (index_in_pipe +1== index_in_p ) )
            {
                wcet=wcet+wcets.at(j);
                i++;
                if(i!=wcets.size()-1)
                {
                    j=i+1;
                    Stage r= s[i];
                    index_in_pipe= r.get_Index_In_Pipe();
                    Stage succesive=s[j];
                    index_in_p= succesive.get_Index_In_Pipe();
                }

            }
        }


        wcets_less_consecutive.push_back(wcet);
        wcet=0;

    }
    for(int i=0; i<wcets_less_consecutive.size(); i++)
    {
        double temp= tot_wcet+ wcets_less_consecutive.at(i);
        tot_wcet=tot_wcet+temp;
    }
    return tot_wcet;
}
double Stage_Set::get_util() const
{
    double u = 0.0;
    for (unsigned i=0; i<s.size(); ++i)
        u += s[i].get_Task().get_wcet() / double(s[i].get_Task().get_period());
    return u;
}
void Stage_Set::clear()
{
    s.clear();
}



}

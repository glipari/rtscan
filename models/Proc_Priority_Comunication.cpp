#include <models/Proc_Priority_Comunication.hpp>
using namespace std;
using namespace Scan;
namespace Scan
{
    Proc_Priority_Comunication::Proc_Priority_Comunication()
    {

    }

    Proc_Priority_Comunication::Proc_Priority_Comunication(const Proc_Priority_Comunication &p)
    {
        for(int i=0; i <p.levels_id_comunication_proc.size();i++)
        {
            vector<int> id_temp;
            for(int j=0; j<p.levels_id_comunication_proc.at(i).size();j++)
            {
                id_temp.push_back(p.levels_id_comunication_proc.at(i).at(j));
            }
            levels_id_comunication_proc.push_back(id_temp);

        }

    }
    void Proc_Priority_Comunication::push_back(vector<int >v)
    {
        levels_id_comunication_proc.push_back(v);
    }
    vector<int> Proc_Priority_Comunication::at (int index)const
    {
        return levels_id_comunication_proc.at(index);
    }
     std::ostream &operator<<(ostream &s, const Proc_Priority_Comunication &p)
{
     s<<"New proc: ";
     s<<endl;
   for(int i=0; i<p.size();i++)
   {
       s<<"Priority level at:";
       s<<i;
       s<<endl;
       vector<int> v=p.at(i);
       for(int j=0;j<v.size();j++)
       {
           s<<v.at(j);
           s<<";";

       }
       s<<endl;
       s<<endl;
   }
    return s;
}


}


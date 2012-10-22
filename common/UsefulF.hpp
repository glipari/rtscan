#ifndef USEFULF_HPP_INCLUDED
#define USEFULF_HPP_INCLUDED
#include <iostream>
#include <models/Processor.hpp>
using namespace std;
using std::endl;
using std::ofstream;

namespace Scan{
 inline double rand_double_lim_sup(double limit)
{
    double divisor = RAND_MAX/(limit+1);
    double retval;

    do
    {
        retval = rand() / divisor;
    }
    while (retval > limit);

    return retval;
}

// Return a random number between lower and upper inclusive.
inline double rand_double_lim(double lower, double upper)
{
    double range = abs(upper-lower);

    return rand_double_lim_sup(range) + lower;
}


inline int rand_lim_sup(int limit)
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
inline int rand_lim(int lower, int upper)
{
    int range = abs(upper-lower);

    return rand_lim_sup(range) + lower;
}

template <class t>
inline void stampa_vettore(std::vector<t>v)
{
    for(int i=0; i<v.size(); i++)
        {
            cout<<v.at(i);
            cout<<" ";
        }
    cout<<endl;
}

inline std::vector <Processor> create_procs_copy(std::vector<Processor> ps)
{
    std::vector<Processor>proc;
    for(int h=0; h<ps.size(); h++)
    {
        proc.push_back(ps.at(h));
        proc.at(proc.size()-1).set_flag_utilised(false);
    }
    return proc;
}
inline std::vector <Processor> create_procs_copy_greedy(std::vector<Processor> ps)
{
    std::vector<Processor>proc;
    for(int h=0; h<ps.size(); h++)
    {
        proc.push_back(ps.at(h));
    }
    return proc;
}

inline std::vector <Processor> create_procs_copy(std::vector<Processor> ps, std::vector<int> v)
{
    std::vector<Processor>proc;
    for(int h=0; h<v.size(); h++)
    {
        proc.push_back(ps.at(v.at(h)));
        proc.at(proc.size()-1).set_flag_utilised(false);
    }
    return proc;
}

inline int get_position_in_proc_vector(int id,std::vector<Processor>p )
{
    for(int i=0; i<p.size(); i++)
    {
        if(p.at(i).get_Id()==id)
            return i;
    }
    return -1;
}



}
#endif // USEFULF_HPP_INCLUDED

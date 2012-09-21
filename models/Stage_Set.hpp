#ifndef RECORD_SET_HPP_INCLUDED
#define RECORD_SET_HPP_INCLUDED


#include <vector>
#include <map>
#include <algorithm>
#include <models/Stage.hpp>
#include <common/exceptions.hpp>
#include <models/task.hpp>

namespace Scan {
    class Stage_Set {

        typedef std::vector<Stage> RLIST;
        /** Set of stage **/
        RLIST s;
    public:
        Stage_Set();
        ~Stage_Set();
        /** Copy constructor */
        Stage_Set(const Stage_Set &ts);
        /** return size of task set */
        int size() const { return s.size(); }

        const Stage& at(unsigned int i) const throw(IndexOutOfBound);

        double get_tot_wcets()const;
        Stage& operator[](unsigned int i) throw (IndexOutOfBound);


        Stage_Set& operator=(const Stage_Set &ts);
        Stage_Set& operator+=(const Stage &t);
        Stage_Set& operator+=(const Stage_Set &t);
        double get_bw_param();

        std::vector<int> get_periods() const;
        std::vector<double> get_dlines() const;
        std::vector<double> get_wcets() const;

        double get_util() const;

        void clear();
    };

template<class Cmp>
void sort_Stage_Sets(std::vector<Stage_Set> rs)
{
    std::sort(rs.begin(),rs.end(), Cmp());
}

class CmpWcetIncr : public std::less<Stage_Set>
{
public:
    bool operator()( const Stage_Set &a,const Stage_Set &b)
    {
       if(a.get_tot_wcets() <= b.get_tot_wcets())
        return true;
        else
        return false;
    }
};


class  CmpWcetDecr : public std::less<Stage_Set>
{
public:
    bool operator()( const Stage_Set &a, const   Stage_Set &b)
    {
        if(a.get_tot_wcets() >= b.get_tot_wcets())
        return true;
        else return false;

    }
};

    Stage_Set operator+(const Stage_Set &ts1, const Stage_Set &ts2);
}
#endif // RECORD_SET_HPP_INCLUDED

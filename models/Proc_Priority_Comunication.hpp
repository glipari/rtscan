#ifndef PROC_PRIORITY_COMUNICATION_HPP_INCLUDED
#define PROC_PRIORITY_COMUNICATION_HPP_INCLUDED
#include<vector>
#include <iostream>



namespace Scan
{
    class Proc_Priority_Comunication
    {
        /** vector contains priority assigned to all other processors by one processor;
        at position i of vector (where i is the level of priority), it is contained
        a vector of int that save all processors id at priority i;**/
        std::vector<std::vector<int>> levels_id_comunication_proc;
        public:
        Proc_Priority_Comunication();
        Proc_Priority_Comunication(const Proc_Priority_Comunication &p);
        void push_back(std::vector<int> v);
        int size()const {return levels_id_comunication_proc.size();};
        std::vector<int> at (int index) const;
    };
    std::ostream &operator<<(std::ostream &s, const Proc_Priority_Comunication &p);
}

#endif // PROC_PRIORITY_COMUNICATION_HPP_INCLUDED

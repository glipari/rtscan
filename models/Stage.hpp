#ifndef RECORD_HPP_INCLUDED
#define RECORD_HPP_INCLUDED
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <models/task.hpp>


namespace Scan
{

class Stage
{
    /** index of pipe in which stage is belong**/
    int index_Pipe;
    /** index of stage in a pipe */
    int index_Task_In_Pipe;
    /** task in this stage */
    Task * task;

public:
    /**
       Default constructor, indexPipe and indexTasInPipe are init to -1 and
       task is initialized using its default constructor.
    */
    Stage();
    /**
           Constructor of Stage.
           @param i index of pipe
           @param j index of taks
           @param t task
    */
    Stage(Task&t, int i, int j);
    /** Copy constructor */
    Stage( const Stage &r);
    /// return index of Pipe
    int get_Index_Pipe();
    /// return index of task inside Pipe
    int get_Index_In_Pipe();
    /// return task
    const Task& get_Task() const;

    bool operator==(const Stage &other) const;


    bool equal(int indexP, int position);



    Stage & operator=(const Stage &r);
};

/**
    Outputs the Stage parameters on ostream. The output consists of
    the index of pipe in which ther is task, the index of task inside the pipe
    and task wcet value .
*/
std::ostream &operator<<(std::ostream &s, Stage &r);

class WcetIncr : public std::less<double>
{
public:
    bool operator()(const double &a,  const  double &b)
    {
        return a <= b;
    }
};
}
#endif // RECORD_HPP_INCLUDED

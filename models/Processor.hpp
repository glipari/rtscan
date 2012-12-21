#ifndef PROCESSOR_HPP_INCLUDED
#define PROCESSOR_HPP_INCLUDED
#include <string>
#include <vector>
#include <common/exceptions.hpp>
#include <iostream>
#include <models/task.hpp>
#include <models/Stage.hpp>
using namespace std;
using namespace Scan;
namespace Scan
{
static int increment=0;
class Processor
{
private:
    double utilised_Bw;/** bw utilized **/
    double starting_bw; /** bw utilized befere allocation **/
    double free; /** free bandwidth **/
    int id; /** processor id **/
    bool used_flag; /** true if processor is used **/
    bool first_task_used;



public:
    /**
       Default constructor: start_Utilised_Bw, utilised_Bw, proc_Utilisation and computation_Time are equal to 0;
       free =1; allocation_List is void;
    */
    Processor();
    /**
       Constructor: start_Utilised_Bw, utilised_Bw are equal to u;
       proc_Utilisation and computation_Time are equal to 0;
       free =1- u;
       allocation_List is void;

       @param u : start value of processor utilisation bandwidth .
    */
    Processor(double u);

    /*** Copy constructor */
    Processor(const Processor &p);

    /// return utilised_Bw
    double get_utilised_Bw()const;
    double get_starting_Bw();
    /// return id processor
    int get_Id() const;
    /// return free
    double get_free()const;

    bool update(double bw);
    void set_flag_utilised(bool f) ;
    bool get_flag_utilised() const;
     Processor& operator=(const Processor &p);
     void subtraction_bw(double bw);
     bool get_first_flag() const;
     bool set_first_flag(bool f);

};
/**
    Outputs the processor parameters on ostream. The output consists of
    utilised_Bw, free, computation_Time, proc_Utilisation and id.
*/
std::ostream &operator<<(ostream &s, Processor &p);


class ProcCmpIdIncr : public std::less<Processor>
{
public:
    bool operator()(const Processor &a,  const  Processor &b)
    {
        return a.get_Id() <= b.get_Id();
    }
};

class ProcCmpUtilizeIncr : public std::less<Processor>
{
public:
    bool operator()(const Processor &a,  const  Processor &b)
    {
        return a.get_free() < b.get_free();
    }
};


class ProcCmpUtilizeDecr : public std::less<Processor>
{
public:
    bool operator()(const Processor &a,  const  Processor &b)
    {
        return a.get_free() > b.get_free();
    }
};

}

#endif // PROCESSOR_HPP_INCLUDED

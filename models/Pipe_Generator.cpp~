#include <models/Pipe_Generator.hpp>
using namespace std;
using std::endl;
using std::ofstream;
namespace Scan
{


double rand_double_lim_sup(double limit)
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
double rand_double_lim(double lower, double upper)
{
    double range = abs(upper-lower);

    return rand_double_lim_sup(range) + lower;
}


int rand_lim_sup(int limit)
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
int rand_lim(int lower, int upper)
{
    int range = abs(upper-lower);

    return rand_lim_sup(range) + lower;
}

Pipe_Generator::Pipe_Generator():filename("Task.txt"), number_of_pipe(5),num_proc(3)
{
    outfile.open(filename);
}
Pipe_Generator::Pipe_Generator(string f,int npipe,int num_pr):filename(f),number_of_pipe(npipe),num_proc(num_pr)
{
    outfile.open(filename);
}

void Pipe_Generator::write_on_file( string line)
{
    outfile << line << endl;
}

int Pipe_Generator::give_num_stage(int num_proc)
{

    double dbl =rand_lim(1,1);
    int num_s=dbl*num_proc;
    return num_s;
}
double Pipe_Generator::compute_period()
{
    int p= rand_lim(1000,1100);
    return p;
}
double Pipe_Generator::compute_deadline(double period)
{


    double factor= rand_double_lim(1,3);
    double deadline=factor*period;
    return deadline;
}


vector<double> Pipe_Generator::compute_wcet(int num_stage,int period)
{
    std::uniform_real_distribution<double> dist(0,1);

    std::vector<double> v;
     double w=0;
    double sum = rand_double_lim(0,1);
    //cout<<"UUUU: "<<sum<<endl;
    for (int i=1; i<=num_stage-1; i++)
    {
        int seed= (int) rand_lim(1,1000);
        rndgen_t *rng = new rndgen_t(seed);
        double next = sum * pow(dist(*rng), 1.0/double(num_stage));
       w=(sum - next)*period;
        v.push_back(w);
        sum = next;
    }
    w=sum*period;
    v.push_back(w);
    return v;

}
void Pipe_Generator::generator()
{
    stringstream ss;
    int number_of_stage=0;
    double period=0;
    string s;
    string p;
    string d;
    double e_t_e_deadline=0;
    for(int i=0; i <number_of_pipe; i++)
    {
        period=compute_period();
        number_of_stage=give_num_stage(num_proc);
        ss<<period;
        p=ss.str();
        ss.str("");

        e_t_e_deadline=compute_deadline(period);

        ss<<e_t_e_deadline;

        d=ss.str();
        ss.str("");
        s=p+","+d+",";
        //cout <<"STRINGA: "<<s<<endl;

        write_on_file(s);
        s.clear();
       // cout <<"DOPO CLEAR: "<<s<<endl;
        vector<double> wcets=compute_wcet(number_of_stage,period);
        double wcet=0;
        for(int j=0; j<number_of_stage; j++)
        {

            wcet= wcets.at(j);
            ss<<wcet;
            s=ss.str();
            ss.str("");
            s+=",";
            s+=d;
            s+=",";
            s+=p;
            s+=",";
            write_on_file(s);
            s.clear();
        }
        s="---";
        write_on_file(s);
        s.clear();


    }
    outfile.close();

}
}

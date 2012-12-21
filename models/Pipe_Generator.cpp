#include <models/Pipe_Generator.hpp>
using namespace std;
using std::endl;
using std::ofstream;
namespace Scan
{



Pipe_Generator::Pipe_Generator(int s):filename("Task.txt"), number_of_pipe(50),seed(s)
{

    outfile.open(filename);
    outf.open("util.txt");
}
Pipe_Generator::Pipe_Generator():filename("Task.txt"), number_of_pipe(50)
{
    seed= rand_lim(1,1100);
    outfile.open(filename);
    outf.open("util.txt");
}
Pipe_Generator::Pipe_Generator(string f, int npipe,int s,double x_u, int n_task):filename(f),number_of_pipe(npipe), seed(s),num_task(n_task), x(x_u)
{
    outfile.open(filename);
}

void Pipe_Generator::write_on_file( string line)
{
    outfile << line << endl;
}

int Pipe_Generator::give_num_stage()
{
    return num_task;
}

double Pipe_Generator::compute_period()
{
    int p= rand_lim(1000,100000);
    return p;
}

double Pipe_Generator::compute_deadline(double period)
{
    double factor= rand_double_lim(1,2);
    double deadline=factor*period;
    return deadline;
}


vector<double> Pipe_Generator::compute_wcet(int num_stage,int period)
{
    std::uniform_real_distribution<double> dist(0,1);
    std::vector<double> v;
    double w=0;
    double inf=0.8*x;
    double sup=1.2*x;

    double sum = rand_double_lim(inf,sup);
    outf<<sum;
    outf<<endl;
    for (int i=1; i<=num_stage-1; i++)
    {
        rndgen_t *rng = new rndgen_t(seed);
        double next = sum * pow(dist(*rng), 1.0/double(num_stage));
        w=(sum - next)*period;
        int n= w;
        v.push_back(n);
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
    int period=0;
    double pe=0;
    double de=0;
    string s;
    string p;
    string d;
    int e_t_e_deadline=0;
    for(int i=0; i <number_of_pipe; i++)
    {
        pe= compute_period();
        period=pe;
        number_of_stage=give_num_stage();
        ss<<period;
        p=ss.str();
        ss.str("");
        de=compute_deadline(period);
        e_t_e_deadline=de;
        ss<<e_t_e_deadline;
        d=ss.str();
        ss.str("");
        s=p+","+d+",";
       write_on_file(s);
        s.clear();
        vector<double> wcets=compute_wcet(number_of_stage,period);
        int wcet=0;
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


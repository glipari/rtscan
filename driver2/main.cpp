
/*#include <gtkmm.h>
#include <gtkmm/main.h>
#include <window.hpp>
*/
#include <models/Sym_Param.hpp>

#include <models/CheckFeasibility.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <common/sequence.hpp>
#include <models/Pipe_Generator.hpp>






using namespace Scan;

/*int main(int argc, char **argv)
{
    Gtk::Main kit(argc, argv);


   ExampleWindow window;

     Gtk::Main::run(window);
    return 0;
}*/bool isEmpty_Task(const string &str)
{
    string endpos = ";";
    if (endpos[0] == str[0])
    {
        return true;
    }
    return false;
}







bool isEmpty_Pipe(const string &str)
{
    string endpos = "---";
    if ((str[0]==endpos[0]) && (str[1]==endpos[1]) && (str[2]==endpos[2]))
    {
        return true;
    }
    return false;
}

void read_proc_file(ifstream &file, Sym_Params &sys)
{
    string line;
    getline(file, line);
   // cout<< "LINE FILE PROC:"<<line<<endl;
    if(line.empty())
    {
        Processor * p1 =new Processor(0);
        Processor * p2 =new Processor(0);
        sys.processors.push_back(*p1);
        sys.processors.push_back(*p2);
    }
    while(!line.empty())
    {
        vector <string>toks;
        string_split(line, ";", back_inserter(toks));
        if (toks.size() != 1) throw "Error, more element not required!";
        else
        {
            double d=atof(toks.at(0).c_str());
            cout <<"***** : "<<d<<endl;
            Processor * p= new Processor(atof(toks.at(0).c_str()));
            sys.processors.push_back(*p);
        }

        line.clear();
        getline(file, line);
      //  cout<< "LINE FILE PROC:"<<line<<endl;
    }
}

void read_input_file(ifstream &file, Sym_Params &sys)
{
    string line;
    string line_inner;

    vector<TaskSet> pipes_data;
    bool firstIter;
    getline(file, line);
    //cout<< "LINE:"<<line<<endl;
    while(!line.empty())
    {
        TaskSet *t= new TaskSet();
        bool firstIter=true;
        while(!isEmpty_Pipe(line))
        {
            vector<string> toks_inner;
            string_split(line, ",", back_inserter(toks_inner));
            int size= toks_inner.size();
            if(firstIter)
            {
                firstIter=false;
                if (size != 2) throw "Not enough elements!";
                double period= atof(toks_inner.at(0).c_str());
                double deadline= atof(toks_inner.at(1).c_str());
                pair<double,double> temp(period,deadline);
                sys.period_deadline.push_back(temp);
            }
            else
            {
                if (size != 3) throw "Not enough elements!";
                const Task *task= new Task(atof(toks_inner.at(0).c_str()), atof(toks_inner.at(1).c_str()), atof(toks_inner.at(2).c_str()));
                *t +=(*task);
            }

            line.clear();
            getline(file, line);
        }
        pipes_data.push_back(*t);
        line.clear();
        getline(file, line);
    }
    sys.pipes_data.insert(sys.pipes_data.begin(), pipes_data.begin(), pipes_data.end());

}
int main(int argc, char **argv)
{

  Sym_Params sys;
    int verbose;
    int type_Alg;
    int taskOrderSelection;
    //Allocation allocation;


    cout<<"Verbose option?"<<endl;
    cout<<"1 - Yes"<<endl;
    cout<<"0 - No"<<endl;
    cin >>verbose;
    if(verbose==1)
        cout<<"Verbose option!"<<endl;
    cout<<"Select the type of algorithm! Digit:"<<endl;
    cout<<"0 - First Fit "<<endl;
    cout<<"1 - Best Fit"<<endl;
    cout<<"2 - Worst Fit"<<endl;
    cin >>type_Alg;
    if(verbose==1)
    {
        cout<<"select alg:"<<type_Alg<<endl;
    }
    cout<<"Select the type of order in which tasks are selected! Digit:"<<endl;
    cout<<"0 - Precedence Order "<<endl;
    cout<<"1 - Decrease Computation Time Order"<<endl;
    cout<<"2 - Increase Computation Time Order"<<endl;
    cin>>taskOrderSelection;
    if(verbose==1)
    {
        cout<<"select task:"<<taskOrderSelection<<endl;
    }
    sys.parse_args(argc, argv);
    sys.print_params();
    cout<< "input file name "<<sys.input_filename.c_str()<<endl;
    ifstream is;
    is.open(sys.input_filename.c_str(),ios_base::in);
    if(is.is_open())
        cout <<"file is open"<<endl;
    else
        cout<<"file is close"<<endl;
    cout<< "input file name processor "<<sys.input_processor_filename.c_str()<<endl;
    ifstream isp;
    isp.open(sys.input_processor_filename.c_str(),ios_base::in);
    if(isp.is_open())
        cout <<"file is open"<<endl;
    else
        cout<<"file is close"<<endl;

    read_input_file(is, sys);
    read_proc_file(isp,sys);
    if (sys.pipes_data.size() == 0)
        cout << "No more task sets to process, exiting!" << endl;

    Pipe_Generator *gen= new Pipe_Generator();
    gen->generator();
    CheckFeasibility* mainClass = new CheckFeasibility( taskOrderSelection , type_Alg, sys.pipes_data,sys.period_deadline, sys.processors,verbose);

    mainClass->check();
   }
    /* a= &(mainClass->check());
      a->printAllocation();
      mainClass->printProcessor();*/

/*

bool isEmpty_Task(const string &str)
{
    string endpos = ";";
    if (endpos[0] == str[0])
    {
        return true;
    }
    return false;
}



bool isEmpty_Pipe(const string &str)
{
    string endpos = "---";
    if ((str[0]==endpos[0]) && (str[1]==endpos[1]) && (str[2]==endpos[2]))
    {
        return true;
    }
    return false;
}

void read_proc_file(ifstream &file, Sym_Params &sys)
{
    string line;
    getline(file, line);
    if(line.empty())
    {
        Processor * p1 =new Processor(0);
        Processor * p2 =new Processor(0);
        sys.processors.push_back(*p1);
        sys.processors.push_back(*p2);
    }
    while(!line.empty())
    {
        vector <string>toks;
        string_split(line, ";", back_inserter(toks));
        if (toks.size() != 1) throw "Error, more element not required!";
        else
        {
            Processor * p= new Processor(atof(toks.at(0).c_str()));
            sys.processors.push_back(*p);
        }

        line.clear();
        getline(file, line);
    }
}

void read_input_file(ifstream &file, Sym_Params &sys)
{
    string line;
    string line_inner;

    vector<TaskSet> pipes_data;
    bool firstIter;
    getline(file, line);
    while(!line.empty())
    {
        TaskSet *t= new TaskSet();
        bool firstIter=true;
        while(!isEmpty_Pipe(line))
        {
            vector<string> toks_inner;
            string_split(line, ",", back_inserter(toks_inner));
            int size= toks_inner.size();
            if(firstIter)
            {
                firstIter=false;
                if (size != 2) throw "Not enough elements!";
                double period= atof(toks_inner.at(0).c_str());
                double deadline= atof(toks_inner.at(1).c_str());
                pair<double,double> temp(period,deadline);
                sys.period_deadline.push_back(temp);
            }
            else
            {
                if (size != 3) throw "Not enough elements!";
                const Task *task= new Task(atof(toks_inner.at(0).c_str()), atof(toks_inner.at(1).c_str()), atof(toks_inner.at(2).c_str()));
                *t +=(*task);
            }

            line.clear();
            getline(file, line);
        }
        pipes_data.push_back(*t);
        line.clear();
        getline(file, line);
    }
    sys.pipes_data.insert(sys.pipes_data.begin(), pipes_data.begin(), pipes_data.end());



}*/




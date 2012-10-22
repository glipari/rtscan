#include <models/window.hpp>

using namespace std;
using namespace Scan;



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

void read_neighbors(ifstream &file, Sym_Params &sys)
{
    string line;
    getline(file, line);
    while(!line.empty())
    {
        vector<int> n;
        vector <string>toks;
        string_split(line, ";", back_inserter(toks));

        for(int j=0; j<toks.size(); j++)
        {
            n.push_back(atoi(toks.at(j).c_str()));
        }
        stampa_vettore(toks);

        sys.push_back_neighbors(n);

        line.clear();
        getline(file, line);
    }
}

void read_proc_file(ifstream &file, Sym_Params &sys)
{
    ifstream ispn;
    ispn.open(sys.input_file_neighbors.c_str(),ios_base::in);
    if(ispn.is_open())
        cout <<"Processors file is open"<<endl;
    else
        cout<<"Processors file is close"<<endl;
    read_neighbors(ispn,sys);
    string line;
    getline(file, line);
    /** if file is empty, we create two processors **/
    if(line.empty())
    {
        Processor * p1 =new Processor(0);
        Processor * p2 =new Processor(0);
        sys.processors.push_back(*p1);
        sys.processors.push_back(*p2);
    }
    int i=0;
    while(!line.empty())
    {
        vector <string>toks;
        string_split(line, ";", back_inserter(toks));
        if (toks.size() != 1) throw "Error, more element not required!";
        else
        {
            double d=atof(toks.at(0).c_str());
            Processor * p;
            if(sys.neighbors.size()>0)
                p= new Processor(atof(toks.at(0).c_str()),sys.neighbors.at(i));
            else
                p= new Processor(atof(toks.at(0).c_str()));
            sys.processors.push_back(*p);
        }
        line.clear();
        getline(file, line);
        i++;
    }
}

void read_task_input_file(ifstream &file, Sym_Params &sys)
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

}

ExampleWindow::ExampleWindow()
    : m_button_check("check"),
    sys(),
    seed(555),
    num_task(6),
    x(0.6),
    m_button_submit("Submit Data"),
    m_CheckButton_Verbose("Verbose"),
    m_CheckButton_reallocataion("Reallocation flag"),
    m_VBox(Gtk::ORIENTATION_VERTICAL),
    m_SBox(Gtk::ORIENTATION_VERTICAL),
    m_Label_fileI("Input File :", true),
    m_Label_fileO("Output File :",true),
    m_Label_fileP("Processor File :", true),
    m_Label_algP("Algorithm of processor selection :",true),
    m_Label_algT("Algorithm of task selection :", true),
    m_Label_seed("Seed :", true),
    m_Label_num_task("Number of tasks per Pipeline :", true),
    m_Label_x("Variable utilization parameter :",true)

{

    set_default_size(100,100);
    set_title("Check Feasibility");
    set_position(Gtk::WIN_POS_CENTER);
    set_border_width(10);
    add(m_VBox);
    m_SBox.add(m_seed_Box);
    m_SBox.add(m_task_Box);
    m_VBox.add(m_H1Box);
    m_VBox.add(m_H2Box);
    m_VBox.add(m_1Box);
    m_VBox.add(m_2Box);
    m_VBox.add(m_3Box);
    m_VBox.add(m_XBox);
    m_VBox.add(m_SBox);
    m_1Box.pack_start(m_Label_fileI);
    m_1Box.pack_start(m_Entry_fileI);
    m_2Box.pack_start(m_Label_fileO);
    m_2Box.pack_start(m_Entry_fileO);
    m_3Box.pack_start(m_Label_fileP);
    m_3Box.pack_start(m_Entry_proc);
    m_H1Box.pack_start( m_Label_algT);
    m_H1Box.pack_start( m_Combo_Task);
    m_H2Box.pack_start( m_Label_algP);
    m_seed_Box.pack_start(m_Label_seed);
    m_seed_Box.pack_start(m_Entry_seed);
    m_task_Box.pack_start(m_Label_num_task);
    m_task_Box.pack_start(m_Entry_num_task);
    m_XBox.pack_start(m_Label_x);
    m_XBox.pack_start(m_Entry_x);
    m_XBox.pack_start(m_CheckButton_reallocataion);
    m_H2Box.pack_start( m_Combo_Alg);
    m_VBox.pack_start(m_button_submit);
    m_VBox.pack_start(m_button_check);
    m_VBox.pack_start(m_CheckButton_Verbose);

    show_all_children();

    m_button_check.signal_clicked().connect(sigc::mem_fun(*this,
                                            &ExampleWindow::on_button_check_clicked) );
    m_button_submit.signal_clicked().connect(sigc::mem_fun(*this,
            &ExampleWindow::on_button_submit_clicked) );
    m_CheckButton_Verbose.signal_toggled().connect( sigc::mem_fun(*this,
            &ExampleWindow::on_checkbox_verbose) );
    m_CheckButton_Verbose.set_active(true);
    m_Combo_Alg.append("Worst Fit");
    m_Combo_Alg.append("Best Fit");
    m_Combo_Alg.append("First Fit");
    m_Combo_Alg.signal_changed().connect(sigc::mem_fun(*this,
                                         &ExampleWindow::on_combo_Alg_changed) );
    m_Combo_Task.append("Precedence Order");
    m_Combo_Task.append("Decrease Computation Time Order");
    m_Combo_Task.append("Increase Computation Time Order");
    m_Combo_Task.signal_changed().connect(sigc::mem_fun(*this,
                                          &ExampleWindow::on_combo_Task_changed) );
    m_Entry_proc.set_max_length(70);
    m_Entry_fileI.set_max_length(70);
    m_Entry_fileO.set_max_length(70);
    m_Entry_proc.set_text("");
    m_Entry_fileI.set_text("");
    m_Entry_fileO.set_text("");
    m_Entry_proc.select_region(0, m_Entry_proc.get_text_length());
    m_Entry_fileI.select_region(0, m_Entry_fileI.get_text_length());
    m_Entry_fileO.select_region(0, m_Entry_fileO.get_text_length());
}

ExampleWindow::~ExampleWindow()
{
    m_Entry_fileI.~Entry();
    m_Entry_fileO.~Entry();
    m_Entry_num_task.~Entry();
    m_Entry_proc.~Entry();
    m_Entry_seed.~Entry();
    m_Entry_x.~Entry();
    m_Combo_Alg.~ComboBoxText();
    m_Combo_Task.~ComboBoxText();
    m_button_check.~Button();
    m_button_submit.~Button();
    m_Label_algP.~Label();
    m_Label_algT.~Label();
    m_Label_fileI.~Label();
    m_Label_fileO.~Label();
    m_Label_fileP.~Label();
    m_Label_num_task.~Label();
    m_Label_seed.~Label();
    m_Label_x.~Label();
    m_CheckButton_reallocataion.~CheckButton();
    m_CheckButton_Verbose.~CheckButton();
    m_SBox.~Box();
    m_VBox.~Box();
    m_1Box.~Box();
    m_2Box.~Box();
    m_3Box.~Box();
    m_XBox.~Box();
}

void ExampleWindow::on_button_check_clicked()
{
    CheckFeasibility* mainClass = new CheckFeasibility( taskOrderSelection , types_Alg, sys.pipes_data,sys.period_deadline, sys.processors,sys.neighbors,verbose);
    mainClass->set_realloc_flag(reallocation_flag);
    mainClass->check();
}

void ExampleWindow::on_button_submit_clicked()
{
    seed= atoi(m_Entry_seed.get_text().c_str());
    num_task= atoi(m_Entry_num_task.get_text().c_str());
    x= atof(m_Entry_x.get_text().c_str());
    input_filename=m_Entry_fileI.get_text();
    input_processor_filename=m_Entry_proc.get_text();
    Pipe_Generator* gen= new Pipe_Generator(input_filename,30,  seed, x, num_task);
    gen->generator();
    ifstream is;
    is.open(input_filename.c_str(),ios_base::in);
    if(is.is_open())
        cout <<"Tasks file is open"<<endl;
    else
        cout<<"Tasks file is close"<<endl;
    ifstream isp;
    isp.open(input_processor_filename.c_str(),ios_base::in);
    read_task_input_file(is, sys);
    read_proc_file(isp,sys);
    if (sys.pipes_data.size() == 0)
        cout << "No more task sets to process, exiting!" << endl;
    return;
}

void ExampleWindow::on_combo_Alg_changed()
{
    Glib::ustring text = m_Combo_Alg.get_active_text();
    if(!(text.empty()))
        std::cout << "Combo changed: " << text << std::endl;
    if(text=="First Fit")
        types_Alg=0;
    else
    {
        if(text=="Best Fit")
            types_Alg=1;
        else
            types_Alg=2;
    }
}

void ExampleWindow::on_combo_Task_changed()
{
    Glib::ustring text = m_Combo_Task.get_active_text();
    if(!(text.empty()))
        std::cout << "Combo changed: " << text << std::endl;
    if(text=="Precedence Order")
        taskOrdersSelection=0;
    else
    {
        if(text=="Decrease Computation Time Order")
            taskOrdersSelection=1;
        else
            taskOrdersSelection=2;
    }
}

void ExampleWindow::on_checkbox_verbose()
{
    verbose=m_CheckButton_Verbose.get_active();

}

void ExampleWindow::on_checkbox_reall()
{
    reallocation_flag=m_CheckButton_reallocataion.get_active();

}




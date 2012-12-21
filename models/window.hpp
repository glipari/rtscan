#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED


#include <gtkmm.h>
#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/checkbutton.h>
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
#include <common/UsefulF.hpp>
#include <models/Proc_Priority_Comunication.hpp>
using namespace std;
using namespace Scan;


class ExampleWindow : public Gtk::Window
{
public:
    ExampleWindow();
    virtual ~ExampleWindow();
protected:

    /* seed used to randomly generate applications */
    int seed;
    /* number of tasks per applicatins */
    int num_task;
    /* variable influence utilization of applications
    * during genartions of them */
    double x;
    /* support class containing some system parameter */
    Sym_Params sys;
    /* type of selection processors policies choisen by the
    user */
    int types_Alg;
    /* type of selection tasks policies choisen by the
    user */
    int taskOrdersSelection;
    /* class supporting options of priority between processors */
    vector<Proc_Priority_Comunication> proc_comunications_prioority_data;
    std::string input_filename;
    bool verbose;
    /* flag indicating if user want option of
    deallocation-reallocation during simulation */
    bool reallocation_flag;
    std::string input_processor_filename;
    std::string input_com_proc_filename;
    /* Signal handlers: start simulation using button check */
    void on_button_check_clicked();
    /* Signal handlers: submit data */
    void on_button_submit_clicked();
    /* Signal handlers: combo box for selection of processors policies */
    void on_combo_Alg_changed();
    /* Signal handlers: combo box for selection of tasks policies */
    void on_combo_Task_changed();
    void on_checkbox_verbose();
    void on_checkbox_reall();

    Gtk::Button m_button_check;
    Gtk::Button m_button_submit;
    Gtk::ComboBoxText m_Combo_Alg;
    Gtk::ComboBoxText m_Combo_Task;
    Gtk::Entry m_Entry_fileI;
    Gtk::Entry m_Entry_fileComProc;
    Gtk::Entry m_Entry_proc;
    Gtk::Entry m_Entry_x;
    Gtk::Entry m_Entry_seed;
    Gtk::Entry m_Entry_num_task;
    Gtk::CheckButton m_CheckButton_Verbose;
    Gtk::CheckButton m_CheckButton_reallocataion;
    Gtk::Label m_Label_algT;
    Gtk::Label m_Label_algP;
    Gtk::Label m_Label_fileI;
    Gtk::Label m_Label_x;
    Gtk::Label m_Label_fileComProc;
    Gtk::Label m_Label_fileP;
    Gtk::Label m_Label_seed;
    Gtk::Label m_Label_num_task;
    Gtk::Box m_H1Box;
    Gtk::Box m_H2Box;
    Gtk::Box m_XBox;
    Gtk::Box m_1Box;
    Gtk::Box m_2Box;
    Gtk::Box m_3Box;
    Gtk::Box m_VBox;
    Gtk::Box m_SBox;
    Gtk::Box m_seed_Box;
    Gtk::Box m_task_Box;
    CheckFeasibility *chk;

};


#endif

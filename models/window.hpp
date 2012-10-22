#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED


#include <gtkmm.h>
#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/checkbutton.h>
#include <Sym_Param.hpp>
#include <CheckFeasibility.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sequence.hpp>
#include <generators.hpp>
#include <Pipe_Generator.hpp>
#include <UsefulF.hpp>
using namespace std;
using namespace Scan;


class ExampleWindow : public Gtk::Window
{
public:
    ExampleWindow();
    virtual ~ExampleWindow();
protected:

    int seed;
    int num_task;
    double x;
    Sym_Params sys;
    int taskOrderSelection;
    int types_Alg;
    int taskOrdersSelection;
    std::string input_filename;
    bool verbose;
    bool reallocation_flag;
    std::string input_processor_filename;
    //Signal handlers:
    void on_button_check_clicked();
    void on_button_submit_clicked();
    void on_combo_Alg_changed();
    void on_combo_Task_changed();
    void on_checkbox_verbose();
    void on_checkbox_reall();

    Gtk::Button m_button_check;
    Gtk::Button m_button_submit;
    Gtk::ComboBoxText m_Combo_Alg;
    Gtk::ComboBoxText m_Combo_Task;
    Gtk::Entry m_Entry_fileI;
    Gtk::Entry m_Entry_fileO;
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
    Gtk::Label m_Label_fileO;
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

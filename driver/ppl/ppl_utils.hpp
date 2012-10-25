#ifndef PPL_UTILS_HPP
#define PPL_UTILS_HPP
#include <ppl.hh>
#include <vector>
#include <iostream>
#include <models/fp_task.hpp>
#include <models/task_parser.hpp>

namespace PPL = Parma_Polyhedra_Library;


bool check_point_contains(const PPL::Pointset_Powerset<PPL::C_Polyhedron> &ps, 
                          int xn, int xd, int yn, int yd);


class SysVisitor : public boost::static_visitor<> {
public:
    std::vector<Scan::FPTask> v;

    SysVisitor() {}

    void operator()(const Scan::Property &p) {
        std::cerr << "ERROR: parsing a property at the sys level" << std::endl;
    }
    void operator()(const Scan::PropertyList &pl) {
        if (pl.type == "sys") 
            for (std::vector< Scan::PropertyList::Element >::const_iterator i = pl.children.begin();
                 i != pl.children.end(); i++) 
                boost::apply_visitor(*this, *i);
        else if (pl.type != "task") 
            std::cerr << "Found a property list which is not a task!" << std::endl;
        else 
            v.push_back(Scan::create_fp_task(pl));
    }
    
};

PPL::Pointset_Powerset<PPL::C_Polyhedron> build_hyperplanes_powerset(std::vector<Scan::FPTask> &v);

#endif

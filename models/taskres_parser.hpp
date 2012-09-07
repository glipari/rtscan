#ifndef TASKRES_PARSER_H_
#define TASKRES_PARSER_H_

#include <istream>
#include <string>
#include <models/taskres.hpp>
#include <models/task_model.hpp>

namespace Scan {
    // DECL_EXC(ParseTaskResExc);
    bool parse_taskres(const std::string &total, std::vector<TaskRes> &tset, std::vector<Resource> &rset);
    void parse_taskres(std::istream &is, const std::string &filename, std::vector<TaskRes> &tset, std::vector<Resource> &rset);
}

#endif /* TASKSET_PARSER_H_ */

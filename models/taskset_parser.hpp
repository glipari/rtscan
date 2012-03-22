/*
 * taskset_parser.h
 *
 *  Created on: May 27, 2011
 *      Author: lipari
 */

#ifndef TASKSET_PARSER_H_
#define TASKSET_PARSER_H_

#include <string>

#include <models/taskset.hpp>
#include <models/task_model.hpp>

namespace Scan {
    DECL_EXC(ParseExc);
    TaskSet parse_tasks(const std::string &total);    
}

#endif /* TASKSET_PARSER_H_ */

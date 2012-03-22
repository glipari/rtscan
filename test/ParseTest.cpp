/*
 * ParseTest.cpp
 *
 *  Created on: May 27, 2011
 *      Author: lipari
 */

#include <gtest/gtest.h>

#include <models/taskset_parser.hpp>

using namespace std;
using namespace Scan;

class ParseTestFix : public ::testing::Test {
protected:
    TaskSet ts;
    std::string str;
    std::vector<std::string> str_err;
    std::vector<std::string> str_ill;

    ParseTestFix() : ts() {
        str = "task pippo { c=1, d=2, p=3, prio = 1 }\n"
            "task pluto { c=1, p=5, prio = 2 } \n"
            "task paperino { c = 1, p = 4 , prio = 7 }\n"
            "task ultimo { c = 1, \n"
            "d = 15,\n"
            "p = 12 \n"
            "}";

        str_err.push_back("task {c = 1}");
        str_err.push_back("{c = 1}");
        str_err.push_back("task pippo {c=1 p=2}");
        str_err.push_back("task pippo {c=1,, p=2}");
        str_err.push_back("task pippo {c=1,, d=2}");
        str_ill.push_back("task illegal1 { c=5, d=10, p=4 }");
        str_ill.push_back("task illegal2 { c=5, d=8 }");
        str_ill.push_back("task illegal2 { c=10 }");
    }
};

TEST_F(ParseTestFix, parseStr)
{
    ts = parse_tasks(str);
    EXPECT_EQ(3, ts[0].get_period());
    EXPECT_EQ(1, ts[0].get_wcet());
    EXPECT_EQ(5, ts[1].get_dline());
    EXPECT_EQ(12, ts["ultimo"].get_period());
    //EXPECT_EQ(7, ts[2].get_property<double>("prio"));
}

TEST_F(ParseTestFix, parseErr)
{
    for (unsigned i=0; i<str_err.size(); ++i)
        EXPECT_THROW(parse_tasks(str_err[i]), ParseExc);
    
    for (unsigned i=0; i<str_ill.size(); ++i)
        EXPECT_THROW(parse_tasks(str_ill[i]), IllegalValue);
}

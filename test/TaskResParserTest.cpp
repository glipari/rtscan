#include <gtest/gtest.h>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <models/taskres_parser.hpp>

using namespace std;
using namespace Scan;

TEST(TaskResParser, Test1)
{
    fstream file("example1.sys");
    
    vector<TaskRes> tset;
    vector<Resource> rset;
    bool resp;
    
    try {
        parse_taskres(file, "example1.sys", tset, rset);
    }
    catch (const std::exception &e2) {
        std::cerr << e2.what() << std::endl;
        FAIL();
    }
}

TEST(TaskResParser, TestExc)
{
    string input = "SYS task pippo { C=2.0 T=10 D=8.0 cs { xx=2 l=3 } } END";
    
    vector<TaskRes> tset;
    vector<Resource> rset;
    bool resp;
    stringstream str(input);
    
    try {
        parse_taskres(str, "Input", tset, rset);
        FAIL();
    }
    catch (std::exception &err) {
        std::cerr << err.what() << endl;
    }
}

TEST(TaskResParser, TestOptional)
{
    string input = "SYS task pippo { C=2.0 T=10 o=3 } END";
    
    vector<TaskRes> tset;
    vector<Resource> rset;
    bool resp;
    stringstream str(input);
    
    try {
        parse_taskres(str, "Input", tset, rset);
        EXPECT_EQ(1, tset.size());
        EXPECT_EQ(10, tset[0].get_period());
        EXPECT_EQ(10, tset[0].get_dline());
        EXPECT_EQ(3, tset[0].get_offset());

    }
    catch (std::exception &err) {
        std::cerr << err.what() << endl;
        FAIL();
    }
}



TEST(TaskResParser, TestResult)
{
    string input = "SYS\n"
                   "res r1 {id=1 short=true}\n" 
                   "res r2 {id=2 short=false}\n" 
        "task t1 { C=2 T=10 D=8 cs { id=1 l=3} }\n"
        "task t2 { C=4 T=25 D=25\n"                
                     "cs { id=1 l=1}\n" 
                     "cs { id=2 l=2 cs {id=1 l=1}}}\n"
        "END";

    vector<TaskRes> tset;
    vector<Resource> rset;
    
    stringstream str(input);

    try {
        parse_taskres(str, "Input", tset, rset);
        EXPECT_EQ(2, rset.size());
        
        EXPECT_EQ(2, tset.size());

        EXPECT_EQ(2, tset[0].get_wcet());
        EXPECT_EQ(10, tset[0].get_period());
        EXPECT_EQ(8, tset[0].get_dline());
        EXPECT_EQ(0, tset[0].get_offset());
        EXPECT_EQ(true, tset[0].uses_resource(1));
        EXPECT_EQ(false, tset[0].uses_resource(2));
        CSSet cslist1 = tset[0].get_outer_cs();
        EXPECT_EQ(1, cslist1.size());
        EXPECT_EQ(3, cslist1[0].get_duration());
        EXPECT_EQ(1, cslist1[0].get_resource());
        
        EXPECT_EQ(4, tset[1].get_wcet());
        EXPECT_EQ(25, tset[1].get_period());
        EXPECT_EQ(25, tset[1].get_dline());
        EXPECT_EQ(0, tset[1].get_offset());
        CSSet cslist2 = tset[1].get_outer_cs();
        EXPECT_EQ(2, cslist2.size());
        EXPECT_EQ(1, cslist2[0].get_duration());
        EXPECT_EQ(1, cslist2[0].get_resource());

        EXPECT_EQ(2, cslist2[1].get_duration());
        EXPECT_EQ(2, cslist2[1].get_resource());
        CSSet cslist3;
        std::copy(cslist2[1].begin(), cslist2[1].end(), std::back_inserter(cslist3));
        EXPECT_EQ(1, cslist3.size());
        EXPECT_EQ(1, cslist3[0].get_resource());
        EXPECT_EQ(1, cslist3[0].get_duration());
    }
    catch (std::exception &err) {
        std::cerr << err.what() << endl;
        FAIL();
    }
}

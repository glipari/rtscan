#include "gtest/gtest.h"

#include <vector>
#include <models/taskres.hpp>
#include <analysis/mbwi_interference.hpp>

using namespace Scan;
using namespace std;

TEST(MBWITest, Simple1)
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    task[0].addCS(CriticalSection(1,2));
    task[1].addCS(CriticalSection(1,5));
    task[2].addCS(CriticalSection(1,7));

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);
    EXPECT_EQ(12, int1);
    EXPECT_EQ(9, int2);
    EXPECT_EQ(7, int3);
}

TEST(MBWITest, Simple2)
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    task[0].addCS(CriticalSection(1,2));
    task[0].addCS(CriticalSection(2,3));

    task[1].addCS(CriticalSection(1,5));
    task[2].addCS(CriticalSection(2,7));


    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);

    EXPECT_EQ(12, int1);
    EXPECT_EQ(2, int2);
    EXPECT_EQ(3, int3);
}

TEST(MBWITest, Nested1)
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    CriticalSection cs(1,5);
    cs.addNestedCS(CriticalSection(2,3));
    task[0].addCS(cs);

    task[1].addCS(CriticalSection(1,4));
    task[2].addCS(CriticalSection(2,7));

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);

    EXPECT_EQ(11, int1);
    EXPECT_EQ(12, int2);
    EXPECT_EQ(3, int3);
}

TEST(MBWITest, Nested2)
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    CriticalSection cs1(1,5);
    cs1.addNestedCS(CriticalSection(2,3));
    task[0].addCS(cs1);

    task[1].addCS(CriticalSection(1,4));

    CriticalSection cs3(1,5);
    cs3.addNestedCS(CriticalSection(2,6));
    task[2].addCS(cs3);

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);

    EXPECT_EQ(5+4, int1);
    EXPECT_EQ(5+5, int2); 
    EXPECT_EQ(5+4, int3);       
}

TEST(MBWITest, Nested3)
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    CriticalSection cs1(1,5);
    cs1.addNestedCS(CriticalSection(2,3));
    task[0].addCS(cs1);

    task[1].addCS(CriticalSection(2,4));

    CriticalSection cs3(1,5);
    cs3.addNestedCS(CriticalSection(2,6));
    task[2].addCS(cs3);

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);

    EXPECT_EQ(5+4+4, int1); // blocked twice by t2
    EXPECT_EQ(3+6, int2); 
    EXPECT_EQ(5+4+4, int3); // blocked twice by t2       
}

// ERROR!
TEST(MBWITest, Nested4)
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    CriticalSection cs1(1,5);
    cs1.addNestedCS(CriticalSection(2,3));
    task[0].addCS(cs1);

    task[1].addCS(CriticalSection(1,3));
    task[1].addCS(CriticalSection(2,4));

    CriticalSection cs3(1,5);
    cs3.addNestedCS(CriticalSection(2,6));
    task[2].addCS(cs3);

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);

    EXPECT_EQ(5+4+3+4, int1); // blocked twice by t2
    EXPECT_EQ(5+5+6, int2);   // on r2, in the worst case it is blocked by t3 
    EXPECT_EQ(5+4+3+4, int3); // blocked twice by t2       
}

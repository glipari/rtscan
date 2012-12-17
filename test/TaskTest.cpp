/*
 * TaskTest.cpp
 *
 *  Created on: May 11, 2011
 *      Author: lipari
 */
#include <gtest/gtest.h>
#include <sstream>

#include <models/task.hpp>
#include <analysis/task_utility.hpp>
#include <models/task_parser.hpp>


using namespace std;
using namespace Scan;

class TaskTestFix : public ::testing::Test {
protected:
    Task *t1;
    Task *t2;

    void SetUp() {
        t1 = new Task();
        t2 = new Task(4.5, 8, 10);
    }
    void TearDown() {
        delete t1;
        delete t2;
    }
};

TEST_F(TaskTestFix, TaskCreate)
{
    Task *t3;
    ASSERT_NO_THROW({
            t3 = new Task(8,4,10);
	});
    delete t3;

    ASSERT_THROW(t3 = new Task(8,12,6), IllegalValue);
    ASSERT_THROW(t3 = new Task(-2,12,6), IllegalValue);
    ASSERT_NO_THROW(t3 = new Task(2,12,6));
    delete t3;
}


TEST_F(TaskTestFix, TaskCopy)
{
    Task t4(*t2);

    ASSERT_EQ(t4.get_period(), 10);
    ASSERT_EQ(t4.get_wcet(), 4.5);
    ASSERT_EQ(t4.get_dline(), 8);
    ASSERT_EQ(t4.get_id(), t2->get_id());

    t4 = *t1;

    ASSERT_EQ(t4.get_period(), 0);
    ASSERT_EQ(t4.get_wcet(), 0);
    ASSERT_EQ(t4.get_dline(), 0);
}

TEST_F(TaskTestFix, TaskPrint)
{
    std::stringstream output;
    output << *t1;
    ASSERT_EQ("0  0  0", output.str());
    output << std::endl << *t2;
    ASSERT_EQ("0  0  0\n4.5  8  10", output.str());
}

TEST_F(TaskTestFix, TaskRead)
{
    std::stringstream input;
    int k;
    std::string task_name;

    input << "task1 25 4 5 6" << std::endl;

    ASSERT_NO_THROW(input >> task_name >> k >> *t1);

    ASSERT_EQ(4, t1->get_wcet());
    ASSERT_EQ(5, t1->get_dline());
    ASSERT_EQ(6, t1->get_period());
}

TEST_F(TaskTestFix, TaskMath)
{
    EXPECT_EQ(30, get_next_arrival(*t2, 25));
    EXPECT_EQ(40, get_next_arrival(*t2, 30));
    EXPECT_EQ(0, get_next_arrival(*t2, -5));

    EXPECT_EQ(20, get_prev_arrival(*t2, 25));
    EXPECT_EQ(30, get_prev_arrival(*t2, 30));
    EXPECT_EQ(-10, get_prev_arrival(*t2, -5));

    EXPECT_EQ(28, get_next_deadline(*t2, 25));
    EXPECT_EQ(38, get_next_deadline(*t2, 29));
    EXPECT_EQ(28, get_next_deadline(*t2, 18));
    EXPECT_EQ(-2, get_next_deadline(*t2, -5));

    EXPECT_EQ(3, get_num_arrived_instances(*t2, 0,25));
    EXPECT_EQ(2, get_num_arrived_instances(*t2, 8,25));
    EXPECT_EQ(2, get_num_arrived_instances(*t2, 10,25));
    EXPECT_EQ(1, get_num_arrived_instances(*t2, 19,28));

    EXPECT_EQ(3, get_num_contained_instances(*t2, 0,28));
    EXPECT_EQ(2, get_num_contained_instances(*t2, 0,25));
    EXPECT_EQ(1, get_num_contained_instances(*t2, 8,25));
    EXPECT_EQ(2, get_num_contained_instances(*t2, 9,29));
}

TEST_F(TaskTestFix, TaskName)
{
    Task task2("MyTask");
    EXPECT_EQ(std::string("MyTask"), task2.get_name());
    Task task3 = task2;
    EXPECT_EQ(std::string("MyTask"), task3.get_name());
}

TEST(TaskTestNoFix, TaskFromProperties)
{
    string spec = "task(mytask){ c=1; T=10; dline=8; };";
    stringstream ss(spec);
    PropertyList pset;
    parse_properties(ss, "string", pset);
    
    EXPECT_EQ("mytask", pset.name);
    EXPECT_EQ("task", pset.type);

    Task task = create_task(pset);
    EXPECT_EQ(1, task.get_wcet());
    EXPECT_EQ(10, task.get_period());
    EXPECT_EQ(8, task.get_dline());
    EXPECT_EQ(0, task.get_jitter());
    EXPECT_EQ(0, task.get_offset());
}

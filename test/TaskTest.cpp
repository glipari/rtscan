/*
 * TaskTest.cpp
 *
 *  Created on: May 11, 2011
 *      Author: lipari
 */
#include "catch.hpp"
#include <sstream>

#include <models/task.hpp>
#include <analysis/task_utility.hpp>
#include <models/task_parser.hpp>
#include <models/fp_task.hpp>


using namespace std;
using namespace Scan;

class TaskTestFix {
public:
    Task *t1;
    Task *t2;

    TaskTestFix() {
        t1 = new Task();
        t2 = new Task(4.5, 8, 10);
    }
    ~TaskTestFix() {
        delete t1;
        delete t2;
    }
};

TEST_CASE("TaskTestFix TaskCreate")
{
    TaskTestFix f;

    Task *t3;
    CHECK_NOTHROW(
            t3 = new Task(8,4,10);
	);
    delete t3;

    CHECK_THROWS_AS(t3 = new Task(8,12,6), IllegalValue);
    CHECK_THROWS_AS(t3 = new Task(-2,12,6), IllegalValue);
    CHECK_NOTHROW(t3 = new Task(2,12,6));
    delete t3;
}


TEST_CASE("TaskTestFix TaskCopy")
{
    TaskTestFix f;

    Task t4(*f.t2);

    REQUIRE(t4.get_period() ==  10);
    REQUIRE(t4.get_wcet() ==  4.5);
    REQUIRE(t4.get_dline() ==  8);
    REQUIRE(t4.get_id() ==  f.t2->get_id());

    t4 = *f.t1;

    REQUIRE(t4.get_period() ==  0);
    REQUIRE(t4.get_wcet() ==  0);
    REQUIRE(t4.get_dline() ==  0);
}

TEST_CASE("TaskTestFix TaskPrint")
{
    TaskTestFix f;

    std::stringstream output;
    output << *f.t1;
    REQUIRE("0  0  0" ==  output.str());
    output << std::endl << *f.t2;
    REQUIRE("0  0  0\n4.5  8  10" ==  output.str());
}

TEST_CASE("TaskTestFix TaskRead")
{
    TaskTestFix f;

    std::stringstream input;
    int k;
    std::string task_name;

    input << "task1 25 4 5 6" << std::endl;

    CHECK_NOTHROW(input >> task_name >> k >> *f.t1);

    REQUIRE(4 ==  f.t1->get_wcet());
    REQUIRE(5 ==  f.t1->get_dline());
    REQUIRE(6 ==  f.t1->get_period());
}

TEST_CASE("TaskTestFix TaskMath")
{
    TaskTestFix f;

    REQUIRE(30 ==  get_next_arrival(*f.t2, 25));
    REQUIRE(40 ==  get_next_arrival(*f.t2, 30));
    REQUIRE(0 ==  get_next_arrival(*f.t2, -5));

    REQUIRE(20 ==  get_prev_arrival(*f.t2, 25));
    REQUIRE(30 ==  get_prev_arrival(*f.t2, 30));
    REQUIRE(-10 ==  get_prev_arrival(*f.t2, -5));

    REQUIRE(28 ==  get_next_deadline(*f.t2, 25));
    REQUIRE(38 ==  get_next_deadline(*f.t2, 29));
    REQUIRE(28 ==  get_next_deadline(*f.t2, 18));
    REQUIRE(-2 ==  get_next_deadline(*f.t2, -5));

    REQUIRE(3 ==  get_num_arrived_instances(*f.t2, 0,25));
    REQUIRE(2 ==  get_num_arrived_instances(*f.t2, 8,25));
    REQUIRE(2 ==  get_num_arrived_instances(*f.t2, 10,25));
    REQUIRE(1 ==  get_num_arrived_instances(*f.t2, 19,28));

    REQUIRE(3 ==  get_num_contained_instances(*f.t2, 0,28));
    REQUIRE(2 ==  get_num_contained_instances(*f.t2, 0,25));
    REQUIRE(1 ==  get_num_contained_instances(*f.t2, 8,25));
    REQUIRE(2 ==  get_num_contained_instances(*f.t2, 9,29));
}

TEST_CASE("TaskTestFix TaskName")
{
    Task task2("MyTask");
    REQUIRE(std::string("MyTask") ==  task2.get_name());
    Task task3 = task2;
    REQUIRE(std::string("MyTask") ==  task3.get_name());
}

TEST_CASE("TaskTestNoFix TaskFromProperties")
{
    string spec = "task(mytask){ c=1; T=10; dline=8; };";
    stringstream ss(spec);
    PropertyList pset;
    parse_properties(ss, "string", pset);
    
    REQUIRE("mytask" ==  pset.name);
    REQUIRE("task" ==  pset.type);

    //Task task = create_task(pset);
    Task task;
    try {
        task.parse(pset);
    } catch (Scan::Exception &exc) {
        cout << "Exception : " << exc.what() << endl;
        FAIL("Stopping here");
    }

    REQUIRE(1 ==  task.get_wcet());
    REQUIRE(10 ==  task.get_period());
    REQUIRE(8 ==  task.get_dline());
    REQUIRE(0 ==  task.get_jitter());
    REQUIRE(0 ==  task.get_offset());
}

TEST_CASE("TaskTestNoFix FPTaskFromProperties")
{
    string spec = "task(mytask){ c=1; T=10; dline=8; prio = 5; };";
    stringstream ss(spec);
    PropertyList pset;
    parse_properties(ss, "string", pset);
    
    FPTask task;
    try {
        task.parse(pset);
    } catch (Scan::Exception &exc) {
        cout << "Exception : " << exc.what() << endl;
        FAIL("Stopping here");
    }

    REQUIRE(5 ==  task.get_priority());
}

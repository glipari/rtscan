#include "catch.hpp"

#include <vector>
#include <algorithm>
#include <models/taskres.hpp>
#include <analysis/mbwi_interference.hpp>

using namespace Scan;
using namespace std;

TEST_CASE("MBWITest, Simple1")
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
    REQUIRE(12 ==  int1);
    REQUIRE(9 ==  int2);
    REQUIRE(7 ==  int3);
}

TEST_CASE("MBWITest, Simple2")
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));

    task[0].addCS(CriticalSection(1,2));
    task[0].addCS(CriticalSection(2,3));

    task[1].addCS(CriticalSection(1,5));
    task[2].addCS(CriticalSection(2,7));


    vector<TaskRes> gamma = compute_gamma(task, 1);
    REQUIRE(2 ==  gamma.size());
    REQUIRE(task[0].get_id() ==  gamma[0].get_id());
    REQUIRE(task[1].get_id() ==  gamma[1].get_id());

    gamma = compute_gamma(task, 2);
    REQUIRE(2 ==  gamma.size());
    REQUIRE(task[0].get_id() ==  gamma[0].get_id());
    REQUIRE(task[2].get_id() ==  gamma[1].get_id());

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);

    REQUIRE((5+7) ==  int1);
    REQUIRE(2 ==  int2);
    REQUIRE(3 ==  int3);
}

TEST_CASE("MBWITest, Nested1")
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

    REQUIRE(11 ==  int1);
    REQUIRE(12 ==  int2);
    REQUIRE(3 ==  int3);
}

TEST_CASE("MBWITest, Nested2")
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

    REQUIRE((5+4) ==  int1);
    REQUIRE((5+5) ==  int2); 
    REQUIRE((5+4) ==  int3);       
}

TEST_CASE("MBWITest, Nested3")
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

    REQUIRE((5+4+4) ==  int1); // blocked twice by t2
    REQUIRE(6 ==  int2); 
    REQUIRE((5+4+4) ==  int3); // blocked twice by t2       
}

TEST_CASE("MBWITest, Permutations")
{
    vector<TaskRes> task;
    
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));
    vector<int> ids;
    for (auto t : task) ids.push_back(t.get_id());

    std::sort(task.begin(), task.end(), LessUniqueId<Task>());
    
    for (int i=0; i<3; i++) REQUIRE(ids[i] ==  task[i].get_id());

    bool f = std::next_permutation(task.begin(), task.end(), LessUniqueId<Task>());
    REQUIRE(true ==  f);
    REQUIRE(ids[0] ==  task[0].get_id());
    REQUIRE(ids[2] ==  task[1].get_id());
    REQUIRE(ids[1] ==  task[2].get_id());

    f = std::next_permutation(task.begin(), task.end(), LessUniqueId<Task>());
    REQUIRE(true ==  f);
    REQUIRE(ids[1] ==  task[0].get_id());
    REQUIRE(ids[0] ==  task[1].get_id());
    REQUIRE(ids[2] ==  task[2].get_id());

    f = std::next_permutation(task.begin(), task.end(), LessUniqueId<Task>());
    REQUIRE(true ==  f);
    REQUIRE(ids[1] ==  task[0].get_id());
    REQUIRE(ids[2] ==  task[1].get_id());
    REQUIRE(ids[0] ==  task[2].get_id());

    f = std::next_permutation(task.begin(), task.end(), LessUniqueId<Task>());
    REQUIRE(true ==  f);
    REQUIRE(ids[2] ==  task[0].get_id());
    REQUIRE(ids[0] ==  task[1].get_id());
    REQUIRE(ids[1] ==  task[2].get_id());

    f = std::next_permutation(task.begin(), task.end(), LessUniqueId<Task>());
    REQUIRE(true ==  f);
    REQUIRE(ids[2] ==  task[0].get_id());
    REQUIRE(ids[1] ==  task[1].get_id());
    REQUIRE(ids[0] ==  task[2].get_id());

    f = std::next_permutation(task.begin(), task.end(), LessUniqueId<Task>());
    REQUIRE(false ==  f);
}

TEST_CASE("MBWITest, Nested4")
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

    REQUIRE((5+4+3+4) ==  int1); // blocked twice by t2
    REQUIRE((5+5+6) ==  int2);   // on r2, in the worst case it is blocked by t3 
    REQUIRE((5+4+3+4) ==  int3); // blocked twice by t2       
}

TEST_CASE("MBWITest, Chained")
{
    vector<TaskRes> task;
    task.push_back(TaskRes(10,50,50));
    task.push_back(TaskRes(20,100,100));
    task.push_back(TaskRes(30,150,150));
    task.push_back(TaskRes(40,200,200));

    CriticalSection cs1(1,5);
    task[0].addCS(cs1);

    CriticalSection cs2(1,4);
    cs2.addNestedCS(CriticalSection(2,3));
    task[1].addCS(cs2);

    CriticalSection cs3(2,6);
    cs3.addNestedCS(CriticalSection(3,2));
    task[2].addCS(cs3);

    task[3].addCS(CriticalSection(3,5));

    double int1 = mbwi_compute_interference(task[0], task);
    double int2 = mbwi_compute_interference(task[1], task);
    double int3 = mbwi_compute_interference(task[2], task);
    double int4 = mbwi_compute_interference(task[3], task);

    REQUIRE((4+6+5) ==  int1);
    REQUIRE((5+6+5) ==  int2);
    REQUIRE((3+5) ==  int3);
    REQUIRE(2 ==  int4);
}

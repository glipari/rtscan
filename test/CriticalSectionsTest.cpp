#include "catch.hpp"

#include <vector>
#include <models/taskres.hpp>

using namespace Scan;
using namespace std;

class TestCSFix {
protected:
    typedef vector<TaskRes> TaskSet;

    vector<Resource> resources;
    TaskSet tasks;
    vector<int> tid;
    
    TestCSFix() {
        // 4 resources
        resources.push_back(Resource(1));
        resources.push_back(Resource(2));
        resources.push_back(Resource(3));
        resources.push_back(Resource(4));
        
        // four tasks
        tasks.push_back(TaskRes(1,20,30));
        tasks.push_back(TaskRes(2,20,30));
        tasks.push_back(TaskRes(3,20,30));
        tasks.push_back(TaskRes(4,20,30));

        for (int i=0; i<4; i++) 
            tid.push_back(tasks[i].get_id());

        /* task 0 */
        CriticalSection cs11(1, 1);        
        CriticalSection cs11_1(2, 3);
        CriticalSection cs11_2(3, 4);
        cs11.addNestedCS(cs11_1);
        cs11.addNestedCS(cs11_2);
        tasks[0].addCS(cs11);
        
        CriticalSection cs12(2, 2);
        CriticalSection cs12_1(3, 5);
        cs12.addNestedCS(cs12_1);
        CriticalSection cs12_2(4, 6);
        cs12.addNestedCS(cs12_2);
        CriticalSection cs12_3(3, 7);
        cs12.addNestedCS(cs12_3);
        tasks[0].addCS(cs12);

        /* task 1 */
        CriticalSection cs21(3, 8);
        CriticalSection cs21_1(4, 9);
        cs21.addNestedCS(cs21_1);
        tasks[1].addCS(cs21);

        /* task 2 */
        CriticalSection cs31(2, 10);
        CriticalSection cs31_1(3, 11);
        CriticalSection cs31_1_1(4, 12);
        cs31_1.addNestedCS(cs31_1_1);
        cs31.addNestedCS(cs31_1);
        tasks[2].addCS(cs31);
    }
};

TEST_CASE_METHOD(TestCSFix, "CopyCS")
{
    CriticalSection c1(1,2);
    CriticalSection c2(c1);

    REQUIRE(1 ==  c2.get_resource());
    REQUIRE(2.0 ==  c2.get_duration());

    int res[] = {2, 3};
    double dur[] = {3.0, 4.0};
 
    int i = 0;
    for (i=0; i<2; i++)
        c1.addNestedCS(CriticalSection(res[i], dur[i]));

    i = 0;
    for (auto i1 = c1.begin(); i1 != c1.end(); i1++, i++) {
        REQUIRE( res[i] ==  i1->get_resource() );
        REQUIRE( dur[i] ==  i1->get_duration() );
        REQUIRE( &c1 ==  i1->get_parent() ); 
    }


    c2 = c1;
    i = 0;
    for (auto i2 = c2.begin(); i2 != c2.end(); i2++, i++) {
        REQUIRE( res[i] ==  i2->get_resource() );
        REQUIRE( dur[i] ==  i2->get_duration() );
        REQUIRE( &c2 ==  i2->get_parent() ); 
    }
}

TEST_CASE_METHOD(TestCSFix, "UsesResource") 
{
    REQUIRE(true ==  tasks[0].uses_resource(1) );
    REQUIRE(true ==  tasks[0].uses_resource(2) );
    REQUIRE(true ==  tasks[0].uses_resource(3) );
    REQUIRE(true ==  tasks[0].uses_resource(4) );
    REQUIRE(false ==  tasks[0].uses_resource(5) );
    REQUIRE(false ==  tasks[0].uses_resource(6) );
}

TEST_CASE_METHOD(TestCSFix, "CSFind") 
{
    CSSet outer1 = tasks[0].get_outer_cs();
    for (auto i = outer1.begin(); i != outer1.end(); i++) {
        REQUIRE(nullptr ==  i->get_parent());
    }
    auto i = outer1.begin();
    const CriticalSection *ptr = i->find_first(1);
    REQUIRE(nullptr != ptr);
    REQUIRE(1 ==  ptr->get_duration());
    ptr = i->find_next();
    REQUIRE(nullptr ==  ptr);

    // calling find_next before find_first returns a null
    ptr = (i+1)->find_next();
    REQUIRE(nullptr ==  ptr);

    // finding a resource not in the tree
    ptr = i->find_first(4);
    REQUIRE(nullptr ==  ptr);

    // a longer list
    ptr = (i+1)->find_first(3);
    REQUIRE(nullptr != ptr);
    REQUIRE(5 ==  ptr->get_duration());
    ptr = (i+1)->find_next();
    REQUIRE(nullptr != ptr);
    REQUIRE(7 ==  ptr->get_duration());
    ptr = (i+1)->find_next();
    REQUIRE(nullptr ==  ptr);
}

TEST_CASE_METHOD(TestCSFix, "CSList") 
{
    CSList l = tasks[0].get_cs_list(1);

    CSList::iterator i = l.begin();
    
    REQUIRE(l.end() != i);

    REQUIRE(1 ==  (*i)->get_duration());
    i++;
    REQUIRE(l.end() ==  i);

    // finding a resource not in the tree
    l = tasks[0].get_cs_list(5);
    REQUIRE(0 ==  l.size());

    l = tasks[0].get_cs_list(3);
    i = l.begin();
    REQUIRE(4 ==  (*i)->get_duration());
    i++;
    REQUIRE(5 ==  (*i)->get_duration());
    i++;
    REQUIRE(7 ==  (*i)->get_duration());
    i++;
    REQUIRE(l.end() ==  i);
}

TEST_CASE_METHOD(TestCSFix, "GetPath")
{        
    // the list of critical section on resource 2
    CSList l = tasks[0].get_cs_list(2);

    // the set of the outermost critical sections
    CSSet s = tasks[0].get_outer_cs();

    // the first in the list 
    CSList::iterator i = l.begin();

    // the first outermost critical section
    CSSet::iterator j = s.begin();
    
    // the path ending in i
    CSList p1 = (*i)->get_path();

    // root of the path, should be the first outermost critical section
    CSList::iterator p_i = p1.begin();

    REQUIRE(1 ==  p1.size());
    REQUIRE(j->get_duration() ==  (*p_i)->get_duration());
    REQUIRE(j->get_resource() ==  (*p_i)->get_resource());

    REQUIRE(p1.begin() ==  find_cs(p1.begin(), p1.end(), 1));

    i++;
    p1 = (*i)->get_path();
    REQUIRE(0 ==  p1.size());
}

TEST_CASE_METHOD(TestCSFix, "FindTaskRes")
{
    REQUIRE(tasks.begin() ==  find_task_uses_res(tasks.begin(), tasks.end(), 1));
    REQUIRE(tasks.end()   ==  find_task_uses_res(tasks.begin()+1, tasks.end(), 1));
    REQUIRE(tasks.end()   ==  find_task_uses_res(tasks.begin(), tasks.end(), 5));
}

TEST_CASE_METHOD(TestCSFix, "TaskSubset")
{
    TaskSet subset;
    subset_tasks_use_res(tasks.begin(), tasks.end(), back_inserter(subset), 1);
    REQUIRE(1 ==  subset.size());
    subset.clear();
    subset_tasks_use_res(tasks.begin(), tasks.end(), back_inserter(subset), 2);
    REQUIRE(2 ==  subset.size());
    REQUIRE(tid[0] ==  subset[0].get_id());
    REQUIRE(tid[2] ==  subset[1].get_id());
}

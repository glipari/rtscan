#include "gtest/gtest.h"

#include <vector>
#include <models/taskres.hpp>

using namespace Scan;
using namespace std;

class TestCSFix : public ::testing::Test {
protected:
    vector<Resource> resources;
    vector<TaskRes> tasks;
    
    TestCSFix() {
        // 4 resources
        resources.push_back(Resource(1));
        resources.push_back(Resource(2));
        resources.push_back(Resource(3));
        resources.push_back(Resource(4));
        
        // four tasks
        tasks.push_back(TaskRes(1,2,3));
        tasks.push_back(TaskRes(1,2,3));
        tasks.push_back(TaskRes(1,2,3));
        tasks.push_back(TaskRes(1,2,3));

        CriticalSection cs11(1, 1);
        CriticalSection cs12(2, 2);
        
        CriticalSection cs11_1(2, 3);
        CriticalSection cs11_2(3, 4);
        cs11.addNestedCS(cs11_1);
        cs11.addNestedCS(cs11_2);
        tasks[0].addCS(cs11);
        
        CriticalSection cs12_1(3, 5);
        cs12.addNestedCS(cs12_1);
        CriticalSection cs12_2(4, 6);
        cs12.addNestedCS(cs12_2);
        CriticalSection cs12_3(3, 7);
        cs12.addNestedCS(cs12_3);
        tasks[0].addCS(cs12);
    }
};

TEST(TESTCS, CopyCS)
{
    CriticalSection c1(1,2);
    CriticalSection c2(c1);

    EXPECT_EQ(1, c2.get_resource());
    EXPECT_EQ(2.0, c2.get_duration());

    int res[] = {2, 3};
    double dur[] = {3.0, 4.0};
 
    int i = 0;
    for (i=0; i<2; i++)
        c1.addNestedCS(CriticalSection(res[i], dur[i]));

    i = 0;
    for (auto i1 = c1.begin(); i1 != c1.end(); i1++, i++) {
        EXPECT_EQ( res[i], i1->get_resource() );
        EXPECT_EQ( dur[i], i1->get_duration() );
        EXPECT_EQ( &c1, i1->get_parent() ); 
    }


    c2 = c1;
    i = 0;
    for (auto i2 = c2.begin(); i2 != c2.end(); i2++, i++) {
        EXPECT_EQ( res[i], i2->get_resource() );
        EXPECT_EQ( dur[i], i2->get_duration() );
        EXPECT_EQ( &c2, i2->get_parent() ); 
    }
}

TEST_F(TestCSFix, UsesResource) 
{
    EXPECT_EQ(true, tasks[0].uses_resource(1) );
    EXPECT_EQ(true, tasks[0].uses_resource(2) );
    EXPECT_EQ(true, tasks[0].uses_resource(3) );
    EXPECT_EQ(true, tasks[0].uses_resource(4) );
    EXPECT_EQ(false, tasks[0].uses_resource(5) );
    EXPECT_EQ(false, tasks[0].uses_resource(6) );
}

TEST_F(TestCSFix, CSFind) 
{
    CSSet outer1 = tasks[0].get_outer_cs();
    for (auto i = outer1.begin(); i != outer1.end(); i++) {
        EXPECT_EQ(nullptr, i->get_parent());
    }
    auto i = outer1.begin();
    const CriticalSection *ptr = i->find_first(1);
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1, ptr->get_duration());
    ptr = i->find_next();
    EXPECT_EQ(nullptr, ptr);

    // calling find_next before find_first returns a null
    ptr = (i+1)->find_next();
    EXPECT_EQ(nullptr, ptr);

    // finding a resource not in the tree
    ptr = i->find_first(4);
    EXPECT_EQ(nullptr, ptr);

    // a longer list
    ptr = (i+1)->find_first(3);
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(5, ptr->get_duration());
    ptr = (i+1)->find_next();
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(7, ptr->get_duration());
    ptr = (i+1)->find_next();
    EXPECT_EQ(nullptr, ptr);
}


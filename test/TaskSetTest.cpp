/*
 * TaskSetTest.cpp
 *
 *  Created on: May 13, 2011
 *      Author: lipari
 */
#include <gtest/gtest.h>

#include <models/taskset.hpp>

using namespace std;
using namespace Scan;

class TaskSetFixture : public ::testing::Test {
protected:
    TaskSet tset1;
    TaskSet tset2;
    Task t1;
    Task t2;
    Task t3;
    Task t4;

    TaskSetFixture() : tset1(), tset2(), t1("TA"), t2("TB"), t3("TC"), t4("TD") {
        t1.set_period(10); t1.set_wcet(1); t1.set_dline(10);
        t2.set_period(20); t2.set_wcet(3); t2.set_dline(9);
        t3.set_period(30); t3.set_wcet(5); t3.set_dline(8);
        t4.set_period(40); t4.set_wcet(7); t4.set_dline(7);

        tset1 += t1;
        tset1 += t2;

        tset2 += t3;
        tset2 += t4;
    }
};

TEST_F(TaskSetFixture, TestAt)
{
    EXPECT_EQ(10, tset1.at(0).get_period());
    EXPECT_EQ(20, tset1.at(1).get_period());

    EXPECT_EQ(10, tset1[0].get_period());
    EXPECT_EQ(20, tset1[1].get_period());

    tset1[0].set_period(5);

    EXPECT_EQ(5, tset1[0].get_period());
    EXPECT_THROW(tset1.at(2), IndexOutOfBound);
    EXPECT_THROW(tset1[2], IndexOutOfBound);

}

TEST_F(TaskSetFixture, TestName)
{
    Task *pt1 = &tset1[0];
    Task *pt2 = &tset1["TA"];
    EXPECT_EQ(pt1, pt2);

    EXPECT_EQ(&tset2[0], &tset2["TC"]);
}

TEST_F(TaskSetFixture, TestSum)
{
    TaskSet tset3;

    tset3 = tset1 + tset2;

    EXPECT_EQ(4, tset3.size());
    EXPECT_NO_THROW(tset3["TA"].get_period());
    EXPECT_NO_THROW(tset3["TB"].get_period());
    EXPECT_NO_THROW(tset3["TC"].get_period());
    EXPECT_NO_THROW(tset3["TD"].get_period());
}

TEST_F(TaskSetFixture, TestSort)
{
    EXPECT_EQ( &tset1[0], &tset1["TA"] );
    EXPECT_EQ( &tset1[1], &tset1["TB"] );

    tset1.sort<TaskCmpDline>();
    EXPECT_EQ( &tset1[0], &tset1["TB"] );
    EXPECT_EQ( &tset1[1], &tset1["TA"] );
    tset1.sort<TaskCmpPeriod>();
    EXPECT_EQ( &tset1[0], &tset1["TA"] );
    EXPECT_EQ( &tset1[1], &tset1["TB"] );
    //tset1["TA"].set_property<int>("priority", 5);
    //tset1["TB"].set_property<int>("priority", 4);

    //tset1.sort_property<int>("priority");
    //EXPECT_EQ( &tset1[0], &tset1["TB"] );
    //EXPECT_EQ( &tset1[1], &tset1["TA"] );
}

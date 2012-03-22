/*
 * TestUtility.cpp
 *
 *  Created on: Jun 6, 2011
 *      Author: lipari
 */

#include "gtest/gtest.h"
#include <memory>
#include <models/task.hpp>
#include <analysis/task_utility.hpp>

using namespace Scan;

TEST(TestUtility,NextArrival)
{
    Task t1 = {1, 5, 8};
    EXPECT_EQ(8, get_next_arrival(t1, 0));
    EXPECT_EQ(8, get_next_arrival(t1, 6));
    EXPECT_EQ(16, get_next_arrival(t1, 8));
    EXPECT_EQ(16, get_next_arrival(t1, 8.1));
    EXPECT_EQ(16, get_next_arrival(t1, 13));

    // offset is 1
    Task t2 = {1, 5, 8, 1};
    EXPECT_EQ(1, t2.get_offset());
    EXPECT_EQ(0, t2.get_jitter());

    EXPECT_EQ(1, get_next_arrival(t2, 0));
    EXPECT_EQ(9, get_next_arrival(t2, 6));
    EXPECT_EQ(9, get_next_arrival(t2, 8));
    EXPECT_EQ(17, get_next_arrival(t2, 9));
    EXPECT_EQ(17, get_next_arrival(t2, 9.1));
    EXPECT_EQ(17, get_next_arrival(t2, 13));

    Task t3 = {1, 5, 8, 1, 4};
    EXPECT_EQ(1, t3.get_offset());
    EXPECT_EQ(4, t3.get_jitter());
    EXPECT_EQ(1, get_next_arrival(t3, 0));
    EXPECT_EQ(9, get_next_arrival(t3, 6));
    EXPECT_EQ(9, get_next_arrival(t3, 8));
    EXPECT_EQ(17, get_next_arrival(t3, 9));
    EXPECT_EQ(17, get_next_arrival(t3, 9.1));
    EXPECT_EQ(17, get_next_arrival(t3, 13));

    Task t4 = {1,5,5};
    EXPECT_EQ(5, get_next_arrival_eq(t4, 5));
    EXPECT_EQ(10, get_next_arrival_eq(t4, 10));
    
}

TEST(TestUtility,NextArrivalWithOffset)
{
    Task t4 = {1, 5, 8, 12};
    EXPECT_EQ(12, t4.get_offset());
    EXPECT_EQ(0, t4.get_jitter());
    EXPECT_EQ(4, get_next_arrival(t4, 0));
    EXPECT_EQ(-4, get_next_arrival(t4, -10));
    EXPECT_EQ(20, get_next_arrival(t4, 12));
    EXPECT_EQ(12, get_next_arrival(t4, 11));
    EXPECT_EQ(20, get_next_arrival(t4, 19));
    EXPECT_EQ(28, get_next_arrival(t4, 21));
    EXPECT_EQ(28, get_next_arrival(t4, 27));   
}

TEST(TestUtility,PrevArrival)
{
    Task t1 = {1, 5, 8};
    EXPECT_EQ(0, get_prev_arrival(t1, 0));
    EXPECT_EQ(0, get_prev_arrival(t1, 7));
    EXPECT_EQ(8, get_prev_arrival(t1, 8));
    EXPECT_EQ(0, get_prev_arrival(t1, 7.999));
    EXPECT_EQ(8, get_prev_arrival(t1, 15.999));
    EXPECT_EQ(16, get_prev_arrival(t1, 16));
}

TEST(TestUtility,PrevArrivalWithOffset)
{
    Task t1 = {1, 5, 8, 5};
    EXPECT_EQ(-3, get_prev_arrival(t1, 0));
    EXPECT_EQ(5, get_prev_arrival(t1, 5));
    EXPECT_EQ(5, get_prev_arrival(t1, 8));
    EXPECT_EQ(5, get_prev_arrival(t1, 12.999));
    EXPECT_EQ(13, get_prev_arrival(t1, 13));
    EXPECT_EQ(21, get_prev_arrival(t1, 22));
}

TEST(TestUtility,PrevArrivalWithLargeOffset)
{
    Task t1 = {1, 5, 8, 20};
    EXPECT_EQ(-4, get_prev_arrival(t1, 0));
    EXPECT_EQ(4, get_prev_arrival(t1, 5));
    EXPECT_EQ(12, get_prev_arrival(t1, 12));
    EXPECT_EQ(20, get_prev_arrival(t1, 20));
    EXPECT_EQ(28, get_prev_arrival(t1, 30));
    EXPECT_EQ(20, get_prev_arrival(t1, 22));
}

TEST(TestUtility,NextDeadline)
{
    Task t1 = {1, 5, 8};

    EXPECT_EQ(5, get_next_deadline(t1, 0));
    EXPECT_EQ(13, get_next_deadline(t1, 5));
    EXPECT_EQ(21, get_next_deadline(t1, 13));

    EXPECT_EQ(13, get_next_deadline(t1, 7));
    EXPECT_EQ(13, get_next_deadline(t1, 8));
    EXPECT_EQ(13, get_next_deadline(t1, 9));

    EXPECT_EQ(-3, get_next_deadline(t1, -4));
    EXPECT_EQ(5, get_next_deadline(t1, -1));
} 

TEST(TestUtility, NextDeadlineWithOffset)
{
    Task t1 = {1, 5, 8, 1};

    EXPECT_EQ(6, get_next_deadline(t1, 0));
    EXPECT_EQ(14, get_next_deadline(t1, 6));
    EXPECT_EQ(22, get_next_deadline(t1, 14));

    EXPECT_EQ(14, get_next_deadline(t1, 7));
    EXPECT_EQ(14, get_next_deadline(t1, 8));
    EXPECT_EQ(14, get_next_deadline(t1, 9));

    EXPECT_EQ(-2, get_next_deadline(t1, -4));
    EXPECT_EQ(6, get_next_deadline(t1, -1));
}

TEST(TestUtility, NextDeadlineWithLargeOffset)
{
    Task t1 = {1, 5, 8, 7};

    EXPECT_EQ(4, get_next_deadline(t1, 0));
    EXPECT_EQ(12, get_next_deadline(t1, 6));
    EXPECT_EQ(20, get_next_deadline(t1, 14));

    EXPECT_EQ(12, get_next_deadline(t1, 7));
    EXPECT_EQ(12, get_next_deadline(t1, 8));
    EXPECT_EQ(12, get_next_deadline(t1, 9));

    EXPECT_EQ(-4, get_next_deadline(t1, -5));
    EXPECT_EQ(4, get_next_deadline(t1, -1));
}

TEST(TestUtility, NextDeadlineWithLargeOffset2)
{
    Task t1 = {1, 5, 8, 15};

    EXPECT_EQ(4, get_next_deadline(t1, 0));
    EXPECT_EQ(12, get_next_deadline(t1, 6));
    EXPECT_EQ(20, get_next_deadline(t1, 14));

    EXPECT_EQ(12, get_next_deadline(t1, 7));
    EXPECT_EQ(12, get_next_deadline(t1, 8));
    EXPECT_EQ(12, get_next_deadline(t1, 9));

    EXPECT_EQ(-4, get_next_deadline(t1, -5));
    EXPECT_EQ(4, get_next_deadline(t1, -1));
}

TEST(TestUtility, DeadlineBeforeAt)
{
    Task t1 = {1, 7, 13};
    
    EXPECT_EQ(7, get_deadline_before_at(t1, 7));
    EXPECT_EQ(20, get_deadline_before_at(t1, 21));
    EXPECT_EQ(33, get_deadline_before_at(t1, 33));

    Task t2 = {1, 10, 10, 5};

    EXPECT_EQ(15, get_deadline_before_at(t2, 20));
    EXPECT_EQ(5, get_deadline_before_at(t2, 10));
    EXPECT_EQ(15, get_deadline_before_at(t2, 15));
}

TEST(TestUtility, ArrivedInstances) 
{
    Task t1 = {1, 5, 8};

    EXPECT_EQ(1, get_num_arrived_instances(t1, 0, 1));
    EXPECT_EQ(0, get_num_arrived_instances(t1, 1, 7));
    EXPECT_EQ(0, get_num_arrived_instances(t1, 7, 8));
    EXPECT_EQ(1, get_num_arrived_instances(t1, 7, 9));
    EXPECT_EQ(2, get_num_arrived_instances(t1, 0, 9));
    EXPECT_EQ(3, get_num_arrived_instances(t1, -8, 9));
}

TEST(TestUtility, ArrivedInstancesWithOffset) 
{
    Task t1 = {1, 5, 8, 3};

    EXPECT_EQ(0, get_num_arrived_instances(t1, 0, 1));
    EXPECT_EQ(1, get_num_arrived_instances(t1, 1, 7));
    EXPECT_EQ(0, get_num_arrived_instances(t1, 4, 10));
    EXPECT_EQ(1, get_num_arrived_instances(t1, 3, 11));
    EXPECT_EQ(2, get_num_arrived_instances(t1, 3, 12));
    EXPECT_EQ(3, get_num_arrived_instances(t1, -5, 12));
}

TEST(TestUtility, ContainedInstancesWithOffset) 
{
    Task t1 = {1, 5, 8, 3};

    EXPECT_EQ(1, get_num_contained_instances(t1, 0, 8));
    EXPECT_EQ(1, get_num_contained_instances(t1, 3, 8));
    EXPECT_EQ(2, get_num_contained_instances(t1, -5, 8));
    EXPECT_EQ(0, get_num_contained_instances(t1, 8, 13));
    EXPECT_EQ(2, get_num_contained_instances(t1, 2, 21));
    EXPECT_EQ(3, get_num_contained_instances(t1, 2, 24));
}


double myfun1(double x) 
{
    return 2 + .5 * x;
}

TEST(TestUtility, FixPoint) 
{
    try {
        EXPECT_EQ(4, fix_point(myfun1, 0));
        EXPECT_EQ(4, fix_point(myfun1, 10));
    } catch (NonMonotonicFunction &err) {
        FAIL() << "Non Monotonic";
    } catch (FixPointDoesNotConverge &err2) {
        FAIL() << "Does not converge";
    }
}

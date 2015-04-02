/*
 * TestUtility.cpp
 *
 *  Created on: Jun 6, 2011
 *      Author: lipari
 */

#include "catch.hpp"
#include <memory>
#include <models/task.hpp>
#include <analysis/task_utility.hpp>
#include <common/floating_point.hpp>

using namespace Scan;
using namespace google_code;

TEST_CASE("TestUtility,NextArrival")
{
    Task t1 = {1, 5, 8};
    REQUIRE(8 ==  get_next_arrival(t1, 0));
    REQUIRE(8 ==  get_next_arrival(t1, 6));
    REQUIRE(16 ==  get_next_arrival(t1, 8));
    REQUIRE(16 ==  get_next_arrival(t1, 8.1));
    REQUIRE(16 ==  get_next_arrival(t1, 13));

    // offset is 1
    Task t2 = {1, 5, 8, 1};
    REQUIRE(1 ==  t2.get_offset());
    REQUIRE(0 ==  t2.get_jitter());

    REQUIRE(1 ==  get_next_arrival(t2, 0));
    REQUIRE(9 ==  get_next_arrival(t2, 6));
    REQUIRE(9 ==  get_next_arrival(t2, 8));
    REQUIRE(17 ==  get_next_arrival(t2, 9));
    REQUIRE(17 ==  get_next_arrival(t2, 9.1));
    REQUIRE(17 ==  get_next_arrival(t2, 13));

    Task t3 = {1, 5, 8, 1, 4};
    REQUIRE(1 ==  t3.get_offset());
    REQUIRE(4 ==  t3.get_jitter());
    REQUIRE(1 ==  get_next_arrival(t3, 0));
    REQUIRE(9 ==  get_next_arrival(t3, 6));
    REQUIRE(9 ==  get_next_arrival(t3, 8));
    REQUIRE(17 ==  get_next_arrival(t3, 9));
    REQUIRE(17 ==  get_next_arrival(t3, 9.1));
    REQUIRE(17 ==  get_next_arrival(t3, 13));

    Task t4 = {1,5,5};
    REQUIRE(5 ==  get_next_arrival_eq(t4, 5));
    REQUIRE(10 ==  get_next_arrival_eq(t4, 10));
    
}

TEST_CASE("TestUtility,NextArrivalWithOffset")
{
    Task t4 = {1, 5, 8, 12};
    REQUIRE(12 ==  t4.get_offset());
    REQUIRE(0 ==  t4.get_jitter());
    REQUIRE(4 ==  get_next_arrival(t4, 0));
    REQUIRE(-4 ==  get_next_arrival(t4, -10));
    REQUIRE(20 ==  get_next_arrival(t4, 12));
    REQUIRE(12 ==  get_next_arrival(t4, 11));
    REQUIRE(20 ==  get_next_arrival(t4, 19));
    REQUIRE(28 ==  get_next_arrival(t4, 21));
    REQUIRE(28 ==  get_next_arrival(t4, 27));   
}

TEST_CASE("TestUtility,PrevArrival")
{
    Task t1 = {1, 5, 8};
    REQUIRE(0 ==  get_prev_arrival(t1, 0));
    REQUIRE(0 ==  get_prev_arrival(t1, 7));
    REQUIRE(8 ==  get_prev_arrival(t1, 8));
    REQUIRE(0 ==  get_prev_arrival(t1, 7.999));
    REQUIRE(8 ==  get_prev_arrival(t1, 15.999));
    REQUIRE(16 ==  get_prev_arrival(t1, 16));
}

TEST_CASE("TestUtility,PrevArrivalWithOffset")
{
    Task t1 = {1, 5, 8, 5};
    REQUIRE(-3 ==  get_prev_arrival(t1, 0));
    REQUIRE(5 ==  get_prev_arrival(t1, 5));
    REQUIRE(5 ==  get_prev_arrival(t1, 8));
    REQUIRE(5 ==  get_prev_arrival(t1, 12.999));
    REQUIRE(13 ==  get_prev_arrival(t1, 13));
    REQUIRE(21 ==  get_prev_arrival(t1, 22));
}

TEST_CASE("TestUtility,PrevArrivalWithLargeOffset")
{
    Task t1 = {1, 5, 8, 20};
    REQUIRE(-4 ==  get_prev_arrival(t1, 0));
    REQUIRE(4 ==  get_prev_arrival(t1, 5));
    REQUIRE(12 ==  get_prev_arrival(t1, 12));
    REQUIRE(20 ==  get_prev_arrival(t1, 20));
    REQUIRE(28 ==  get_prev_arrival(t1, 30));
    REQUIRE(20 ==  get_prev_arrival(t1, 22));
}

TEST_CASE("TestUtility,NextDeadline")
{
    Task t1 = {1, 5, 8};

    REQUIRE(5 ==  get_next_deadline(t1, 0));
    REQUIRE(13 ==  get_next_deadline(t1, 5));
    REQUIRE(21 ==  get_next_deadline(t1, 13));

    REQUIRE(13 ==  get_next_deadline(t1, 7));
    REQUIRE(13 ==  get_next_deadline(t1, 8));
    REQUIRE(13 ==  get_next_deadline(t1, 9));

    REQUIRE(-3 ==  get_next_deadline(t1, -4));
    REQUIRE(5 ==  get_next_deadline(t1, -1));
} 

TEST_CASE("TestUtility, NextDeadlineWithOffset")
{
    Task t1 = {1, 5, 8, 1};

    REQUIRE(6 ==  get_next_deadline(t1, 0));
    REQUIRE(14 ==  get_next_deadline(t1, 6));
    REQUIRE(22 ==  get_next_deadline(t1, 14));

    REQUIRE(14 ==  get_next_deadline(t1, 7));
    REQUIRE(14 ==  get_next_deadline(t1, 8));
    REQUIRE(14 ==  get_next_deadline(t1, 9));

    REQUIRE(-2 ==  get_next_deadline(t1, -4));
    REQUIRE(6 ==  get_next_deadline(t1, -1));
}

TEST_CASE("TestUtility, NextDeadlineWithLargeOffset")
{
    Task t1 = {1, 5, 8, 7};

    REQUIRE(4 ==  get_next_deadline(t1, 0));
    REQUIRE(12 ==  get_next_deadline(t1, 6));
    REQUIRE(20 ==  get_next_deadline(t1, 14));

    REQUIRE(12 ==  get_next_deadline(t1, 7));
    REQUIRE(12 ==  get_next_deadline(t1, 8));
    REQUIRE(12 ==  get_next_deadline(t1, 9));

    REQUIRE(-4 ==  get_next_deadline(t1, -5));
    REQUIRE(4 ==  get_next_deadline(t1, -1));
}

TEST_CASE("TestUtility, NextDeadlineWithLargeOffset2")
{
    Task t1 = {1, 5, 8, 15};

    REQUIRE(4 ==  get_next_deadline(t1, 0));
    REQUIRE(12 ==  get_next_deadline(t1, 6));
    REQUIRE(20 ==  get_next_deadline(t1, 14));

    REQUIRE(12 ==  get_next_deadline(t1, 7));
    REQUIRE(12 ==  get_next_deadline(t1, 8));
    REQUIRE(12 ==  get_next_deadline(t1, 9));

    REQUIRE(-4 ==  get_next_deadline(t1, -5));
    REQUIRE(4 ==  get_next_deadline(t1, -1));
}

TEST_CASE("TestUtility, DeadlineBeforeAt")
{
    Task t1 = {1, 7, 13};
    
    REQUIRE(7 ==  get_deadline_before_at(t1, 7));
    REQUIRE(20 ==  get_deadline_before_at(t1, 21));
    REQUIRE(33 ==  get_deadline_before_at(t1, 33));

    Task t2 = {1, 10, 10, 5};

    REQUIRE(15 ==  get_deadline_before_at(t2, 20));
    REQUIRE(5 ==  get_deadline_before_at(t2, 10));
    REQUIRE(15 ==  get_deadline_before_at(t2, 15));
}

TEST_CASE("TestUtility, ArrivedInstances") 
{
    Task t1 = {1, 5, 8};

    REQUIRE(1 ==  get_num_arrived_instances(t1, 0, 1));
    REQUIRE(0 ==  get_num_arrived_instances(t1, 1, 7));
    REQUIRE(0 ==  get_num_arrived_instances(t1, 7, 8));
    REQUIRE(1 ==  get_num_arrived_instances(t1, 7, 9));
    REQUIRE(2 ==  get_num_arrived_instances(t1, 0, 9));
    REQUIRE(3 ==  get_num_arrived_instances(t1, -8, 9));
}

TEST_CASE("TestUtility, ArrivedInstancesWithOffset") 
{
    Task t1 = {1, 5, 8, 3};

    REQUIRE(0 ==  get_num_arrived_instances(t1, 0, 1));
    REQUIRE(1 ==  get_num_arrived_instances(t1, 1, 7));
    REQUIRE(0 ==  get_num_arrived_instances(t1, 4, 10));
    REQUIRE(1 ==  get_num_arrived_instances(t1, 3, 11));
    REQUIRE(2 ==  get_num_arrived_instances(t1, 3, 12));
    REQUIRE(3 ==  get_num_arrived_instances(t1, -5, 12));
}

TEST_CASE("TestUtility, ContainedInstancesWithOffset") 
{
    Task t1 = {1, 5, 8, 3};

    REQUIRE(1 ==  get_num_contained_instances(t1, 0, 8));
    REQUIRE(1 ==  get_num_contained_instances(t1, 3, 8));
    REQUIRE(2 ==  get_num_contained_instances(t1, -5, 8));
    REQUIRE(0 ==  get_num_contained_instances(t1, 8, 13));
    REQUIRE(2 ==  get_num_contained_instances(t1, 2, 21));
    REQUIRE(3 ==  get_num_contained_instances(t1, 2, 24));
}


double myfun1(double x) 
{
    return 2 + .5 * x;
}

TEST_CASE("TestUtility, FixPoint") 
{
    try {
        FloatingPoint<double> res1(fix_point(myfun1, 0));
        FloatingPoint<double> res2(fix_point(myfun1, 10));
        FloatingPoint<double> ex(4);
        
        CHECK(ex.AlmostEquals(res1));
        CHECK(ex.AlmostEquals(res2));
    } catch (NonMonotonicFunction &err) {
        FAIL("Non Monotonic");
    } catch (FixPointDoesNotConverge &err2) {
        FAIL("Does not converge");
    }
}

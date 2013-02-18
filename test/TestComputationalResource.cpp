/*
 *  TestComputationalResource.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: Youcheng Sun
 */

#include "gtest/gtest.h"
#include <models/computational_resource.hpp>
#include <vector>

using namespace Scan;
using namespace std;

TEST(TestComputationalResource, Enqueue)
{
    ComputationalResource node;

    FPTask_ptr t1( new FPTask(1, 10, 10));
    FPTask_ptr t2( new FPTask(1, 5, 5));
    FPTask_ptr t3( new FPTask(1, 20, 20));

    t1->set_sched("fp");
    t2->set_sched("fp");
    t3->set_sched("fp");
    t1->set_priority(2);
    t2->set_priority(3);
    t3->set_priority(1);

    node.register_a_fp_task(t1);
    node.register_a_fp_task(t2);
    node.register_a_fp_task(t3);

    vector<FPTask_ptr> v = node.get_rq();

    EXPECT_EQ( t2->get_period(), v[0]->get_period());
    EXPECT_EQ( t1->get_period(), v[1]->get_period());
    EXPECT_EQ( t3->get_period(), v[2]->get_period());
}

TEST(TestComputationalResource, Enqueue_NP)
{
    ComputationalResource node;

    FPTask_ptr t1( new FPTask(1, 10, 10));
    FPTask_ptr t2( new FPTask(1, 5, 5));
    FPTask_ptr t3( new FPTask(1, 20, 20));

    t1->set_sched("npfp");
    t2->set_sched("npfp");
    t3->set_sched("npfp");
    t1->set_priority(2);
    t2->set_priority(3);
    t3->set_priority(1);

    node.register_a_fp_task(t1);
    node.register_a_fp_task(t2);
    node.register_a_fp_task(t3);

    vector<FPTask_ptr> v = node.get_rq();

    EXPECT_EQ( t2->get_period(), v[0]->get_period());
    EXPECT_EQ( t1->get_period(), v[1]->get_period());
    EXPECT_EQ( t3->get_period(), v[2]->get_period());
}

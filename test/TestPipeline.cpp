/*
 *  TestPipeline.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: Youcheng Sun
 */

#include "gtest/gtest.h"
#include <models/pipeline.hpp>
#include <vector>

using namespace Scan;
using namespace std;

TEST(TestPipeline, PlineTag)
{
    Pipeline p1(5, 5); 
    Pipeline p2(10, 10); 
    Pipeline p3(20, 20); 

    FPTask_ptr t1( new FPTask());
    t1->set_pipeline_pos(0);
    p1.register_a_fp_task(t1);

    EXPECT_EQ(t1->get_pipeline_tag(), p1.get_tag());
}

TEST(TestPipeline, PlinePos)
{
    Pipeline pline(10, 10);

    FPTask_ptr t1( new FPTask(1, 2, 2));
    FPTask_ptr t2( new FPTask(1, 3, 3));
    FPTask_ptr t3( new FPTask(1, 4, 4));
    t1->set_name("t1");
    t1->set_pipeline_pos(2);
    t2->set_name("t2");
    t2->set_pipeline_pos(0);
    t3->set_name("t3");
    t3->set_pipeline_pos(1);

    pline.register_a_fp_task(t1);
    pline.register_a_fp_task(t2);
    pline.register_a_fp_task(t3);

    const vector<FPTask_ptr> &v = pline.pline_tasks;

    EXPECT_EQ(v[0]->get_pipeline_pos(), 0);
    EXPECT_EQ(v[1]->get_pipeline_pos(), 1);
    EXPECT_EQ(v[2]->get_pipeline_pos(), 2);

    EXPECT_EQ(v[0]->get_name(), t2->get_name());
    EXPECT_EQ(v[1]->get_name(), t3->get_name());
    EXPECT_EQ(v[2]->get_name(), t1->get_name());
}

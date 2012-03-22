#include "gtest/gtest.h"

#include <vector>
#include <analysis/fp_response_time.hpp>

using namespace Scan;
using namespace std;

class TestFPFix : public ::testing::Test {
protected:
    vector<FPTask> v;
    vector<FPTask> v2;
    vector<FPTask> v3;

    TestFPFix() : v() {
        v.push_back(FPTask(1, 4, 5));
        v.push_back(FPTask(1, 3, 10));
        v.push_back(FPTask(1, 2, 2));
        v.push_back(FPTask(1, 1, 4));

        v2.push_back(FPTask(1, 5, 5));
        v2.push_back(FPTask(2, 10, 10));
        v2.push_back(FPTask(4, 30, 30));
        v2.push_back(FPTask(7, 40, 40));
        v2.push_back(FPTask(10, 100, 100));

        v3.push_back(FPTask( 1,  4,  4));
        v3.push_back(FPTask( 2, 10, 10));
        v3.push_back(FPTask( 3, 30, 30));
        v3.push_back(FPTask( 5, 15, 15));
        v3.push_back(FPTask(10,100,100));
        int i=5;
        for (auto c : v3) 
            c.set_priority(i--);
    }
};

TEST(TestFP, Conversion)
{
    FPTask t(2, 5, 10);
    t.set_priority(3);
    EXPECT_EQ(3, t.get_priority());
    Task t2 = t;
    EXPECT_EQ(t.get_wcet(), t2.get_wcet());

    Task t3;
    t3 = t;
    EXPECT_EQ(t.get_dline(), t3.get_dline());
    EXPECT_EQ(t2.get_period(), t3.get_period());
}

TEST_F(TestFPFix, AssignRM)
{
    assign_rm_priorities(v.begin(), v.end());

    EXPECT_EQ( 2, v[0].get_period());
    EXPECT_EQ(10, v[3].get_period());
    EXPECT_EQ( 4, v[0].get_priority()); 
    EXPECT_EQ( 1, v[3].get_priority()); 
}

TEST_F(TestFPFix, AssignDM) 
{
    assign_dm_priorities(v.begin(), v.end());

    EXPECT_EQ(1, v[0].get_dline());
    EXPECT_EQ(4, v[3].get_dline());
    EXPECT_EQ(4, v[0].get_priority()); 
    EXPECT_EQ(1, v[3].get_priority()); 
}

TEST_F(TestFPFix, RespTime) 
{
    assign_rm_priorities(v2.begin(), v2.end());

    EXPECT_EQ( 1,resp_time(v2.begin(), v2.begin()+1, 100));
    EXPECT_EQ( 3,resp_time(v2.begin(), v2.begin()+2, 100));
    EXPECT_EQ( 8,resp_time(v2.begin(), v2.begin()+3, 100));
    EXPECT_EQ(19,resp_time(v2.begin(), v2.begin()+4, 100));
    EXPECT_EQ(55,resp_time(v2.begin(), v2.begin()+5, 100));    

    sort_by_decreasing(v3.begin(), v3.end(), &FPTask::get_priority);

    EXPECT_EQ( 1,resp_time(v3.begin(), v3.begin()+1, 100));
    EXPECT_EQ( 3,resp_time(v3.begin(), v3.begin()+2, 100));
    EXPECT_EQ( 7,resp_time(v3.begin(), v3.begin()+3, 100));
    EXPECT_EQ(16,resp_time(v3.begin(), v3.begin()+4, 100));
    EXPECT_EQ(90,resp_time(v3.begin(), v3.begin()+5, 100));    
}

TEST_F(TestFPFix, Align)
{
    vector<FPTask> v = { {1,5,5,4},
                          {1,8,8,7},
                          {1,9,9,2} };
    auto vv = v;
 
    align_critical_instant(vv.begin(), vv.end());
    EXPECT_EQ( 0, vv[0].get_offset());
    EXPECT_EQ( 0, vv[1].get_offset());
    EXPECT_EQ( 0, vv[2].get_offset());

    EXPECT_EQ( 4, v[0].get_offset());
    EXPECT_EQ( 7, v[1].get_offset());
    EXPECT_EQ( 2, v[2].get_offset());
} 

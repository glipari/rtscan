#include "gtest/gtest.h"

#include <vector>
#include <analysis/fp_response_time.hpp>

using namespace Scan;
using namespace std;

class TestFPPtrFix : public ::testing::Test {
protected:
    vector<FPTask_ptr> v;
    vector<FPTask_ptr> v2;
    vector<FPTask_ptr> v3;

    TestFPPtrFix() : v() {
        v.push_back(FPTask_ptr( new FPTask(1, 4, 5)));
        v.push_back(FPTask_ptr( new FPTask(1, 3, 10)));
        v.push_back(FPTask_ptr( new FPTask(1, 2, 2)));
        v.push_back(FPTask_ptr( new FPTask(1, 1, 4)));

        v2.push_back(FPTask_ptr( new FPTask(1, 5, 5)));
        v2.push_back(FPTask_ptr( new FPTask(2, 10, 10)));
        v2.push_back(FPTask_ptr( new FPTask(4, 30, 30)));
        v2.push_back(FPTask_ptr( new FPTask(7, 40, 40)));
        v2.push_back(FPTask_ptr( new FPTask(10, 100, 100)));

        v3.push_back(FPTask_ptr( new FPTask( 1,  4,  4)));
        v3.push_back(FPTask_ptr( new FPTask( 2, 10, 10)));
        v3.push_back(FPTask_ptr( new FPTask( 3, 30, 30)));
        v3.push_back(FPTask_ptr( new FPTask( 5, 15, 15)));
        v3.push_back(FPTask_ptr( new FPTask(10,100,100)));
        int i=5;
        for (auto c : v3) 
            c->set_priority(i--);
    }
};

TEST(TestFPPtr, Conversion)
{
    FPTask_ptr t( new FPTask(2, 5, 10));
    t->set_priority(3);
    EXPECT_EQ(3, t->get_priority());
    Task t2 = *t;
    EXPECT_EQ(t->get_wcet(), t2.get_wcet());

    Task t3;
    t3 = *t;
    EXPECT_EQ(t->get_dline(), t3.get_dline());
    EXPECT_EQ(t2.get_period(), t3.get_period());
}

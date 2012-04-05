#include <vector>
#include <gtest/gtest.h>
#include <models/task_set.hpp>
#include <models/task.hpp>

using namespace std;
using namespace Scan;

TEST(TaskSet2, IntersectSubtract)
{
    vector<int> v = {
        1,2,3,4,5
    };

    vector<int> v1(v.begin(), v.begin()+3);
    vector<int> v2(v.begin()+2, v.end());

    EXPECT_EQ(3, v1.size());
    EXPECT_EQ(3, v2.size());

    vector<int> inter;
    intersect(v1.begin(), v1.end(), v2.begin(), v2.end(), inter);

    EXPECT_EQ(1, inter.size());
    EXPECT_EQ(3, inter[0]);

    vector<int> sub;
    subtract(v1.begin(), v1.end(), v2.begin(), v2.end(), sub);
    EXPECT_EQ(2, sub.size());
    EXPECT_EQ(1, sub[0]);
    EXPECT_EQ(2, sub[1]);
}

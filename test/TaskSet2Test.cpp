#include <vector>
#include <gtest/gtest.h>
#include <common/sequence.hpp>
#include <models/task.hpp>

using namespace std;
using namespace Scan;

TEST(TaskSet2, IntersectSubtract)
{
    vector<Task> v = {
        Task(1,10,10),
        Task(2,10,10),
        Task(3,10,10),
        Task(4,10,10),
        Task(5,10,10)
    };

    vector<Task> v1(v.begin(), v.begin()+3);
    vector<Task> v2(v.begin()+2, v.end());
    
    EXPECT_EQ(3, v1.size());
    EXPECT_EQ(3, v2.size());

    vector<Task> inter;
    intersect(inter, v1.begin(), v1.end(), v2.begin(), v2.end(), task_cmp_ids);

    EXPECT_EQ(1, inter.size());
    EXPECT_EQ(v[2].get_id(), inter[0].get_id());

    vector<Task> sub;
    subtract(sub, v1.begin(), v1.end(), v2.begin(), v2.end(), task_cmp_ids);
    EXPECT_EQ(2, sub.size());
    EXPECT_EQ(v[0].get_id(), sub[0].get_id());
    EXPECT_EQ(v[1].get_id(), sub[1].get_id());
}

TEST(Sequence, TestMaxSum)
{
    vector<double> v = {
        4, 7, 10, 2, 3, 1, -5
    };
    
    EXPECT_EQ(21, sum_max_n(v.begin(), v.end(), 3));
}

#include <vector>
#include <list>
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
    intersect(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(inter), 
              cmp_ids<Task>);

    EXPECT_EQ(1, inter.size());
    EXPECT_EQ(v[2].get_id(), inter[0].get_id());

    vector<Task> sub;
    subtract(v1.begin(), v1.end(), v2.begin(), v2.end(),
             back_inserter(sub),
             cmp_ids<Task>);
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

TEST(Sequence, SplitString)
{
    string str = "10    20";
    vector<string> res;
    string_split(str, " ", back_inserter(res));
    EXPECT_EQ(2, res.size());

    str = "10,,, 20";
    res.clear();
    string_split(str, " ", back_inserter(res));
    EXPECT_EQ(2, res.size());

    res.clear();
    string_split(str, " ,", back_inserter(res));
    EXPECT_EQ(2, res.size());

    res.clear();
    string_split(str, " ,", back_inserter(res), true);
    EXPECT_EQ(5, res.size());

    str = "10   ";
    res.clear();
    string_split(str, " ", back_inserter(res));
    EXPECT_EQ(1, res.size());


}

TEST(Sequence, Select2)
{
    vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    list<int> r;

    select(v.begin(), v.end(), back_inserter(r), [](int x) { return x%2; });
    auto l = r.begin();
    for (int i=0; i<5;i++) {
        EXPECT_EQ(2*i+1, *l);
        ++l;
    }
}

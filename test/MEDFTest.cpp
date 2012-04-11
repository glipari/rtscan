#include "gtest/gtest.h"

#include <analysis/medf_rta.hpp>

using namespace std;
using namespace Scan;

TEST(MEDFRTA, Simple1)
{
    vector<FPTask> tset = {
        FPTask(5,20,20),
        FPTask(4,16,16),
        FPTask(7,28,28), 
        FPTask(9,36,36)
    };

    EXPECT_EQ(true, gedf_iterative(2, tset.begin(), tset.end()));
    EXPECT_EQ(true, gedf_bound_gfb(2, tset.begin(), tset.end()));
    EXPECT_EQ(true, gfp_bound_density(2, tset.begin(), tset.end()));
}

TEST(MEDFRTA, Simple2)
{
    vector<Task> tset = {
        Task(4,6,6),
        Task(4,6,6),
        Task(4,6,6)
    };

    EXPECT_EQ(false, gedf_iterative(2, tset.begin(), tset.end()));
    EXPECT_EQ(false, gedf_bound_gfb(2, tset.begin(), tset.end()));
    EXPECT_EQ(false, gfp_bound_density(2, tset.begin(), tset.end()));
}


TEST(MEDFRTA, Simple2)
{
    vector<Task> tset = {
        Task(4,6,6),
        Task(4,6,6),
        Task(4,6,6)
    };

    EXPECT_EQ(false, gedf_iterative(2, tset.begin(), tset.end()));
    EXPECT_EQ(false, gedf_bound_gfb(2, tset.begin(), tset.end()));
    EXPECT_EQ(false, gfp_bound_density(2, tset.begin(), tset.end()));
}


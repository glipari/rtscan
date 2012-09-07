#include "gtest/gtest.h"

#include <models/fp_task.hpp>
#include <analysis/hyperplane.hpp>

using namespace Scan;
using namespace std;

TEST(TestHET, Points1)
{
    vector<FPTask> tset = { {2, 5, 10}, {3, 12, 15} };

    vector<double> p = compute_points(tset.begin(), tset.end(), 12);

    EXPECT_EQ(2, p.size());
    EXPECT_EQ(p[0], 10);
    EXPECT_EQ(p[1], 12);
}

TEST(TestHET, PositiveSpace)
{
    for (int n=2; n<10; n++) {
        space_t s = positive_space(n);
        for (int i=0; i<n; ++i) {
            for (int j=0; j<n; ++j) {
                if (j != i) 
                    EXPECT_EQ(0, s[i].a[j]);
                else 
                    EXPECT_EQ(1, s[i].a[j]);
            }
            EXPECT_EQ(1, s[i].sign);
            EXPECT_EQ(0, s[i].b);
        }
    }
}

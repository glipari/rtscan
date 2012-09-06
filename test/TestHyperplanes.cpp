#include "gtest/gtest.h"

#include <analysis/hyperplanes.hpp>

using namespace Scan;
using namespace std;

TEST(TestHET, Points1)
{
    vector<FPTask> tset = { {2, 5, 10}, {3, 12, 15} };

    vector<double> p = compute_points(tset.begin(), tset.end(), 12);

    EXPECT_EQ(2, p.size());
    EXPECT_EQ(p[0], 10);
    EXPECT_EQ(p[0], 12);
}

#include "gtest/gtest.h"
#include <vector>
#include <common/generators.hpp>

using namespace std;
using namespace Scan;

TEST(Generators, Cartesian)
{
    vector< pair<int, int> > v = {
        {1,3},
        {1,3}
    };
    
    vector< vector<int> > elems = cartesian_product(v);
    EXPECT_EQ(9, elems.size());
    pair<int, int> p = {1,1};
    EXPECT_EQ(p.first, elems[0][0]);
    EXPECT_EQ(p.second, elems[0][1]);
    p = {1,2};
    EXPECT_EQ(p.first, elems[1][0]);
    EXPECT_EQ(p.second, elems[1][1]);
    p = {1,3};
    EXPECT_EQ(p.first, elems[2][0]);
    EXPECT_EQ(p.second, elems[2][1]);
    p = {2,1};
    EXPECT_EQ(p.first, elems[3][0]);
    EXPECT_EQ(p.second, elems[3][1]);

    p = {3,3};
    EXPECT_EQ(p.first, elems[8][0]);
    EXPECT_EQ(p.second, elems[8][1]);
    
    vector< vector<int> > good = select_sum_vectors(elems, 3);
    EXPECT_EQ(2, good.size());

    good = select_sum_vectors(elems, 7);
    EXPECT_EQ(0, good.size());
}

TEST(Generators, SelectSum)
{
    vector< pair<int, int> > v1 = {
        {0,3},
        {1,3}
    };
    
    vector< vector<int> > elems = cartesian_product(v1);
    vector< vector<int> > good = select_sum_vectors(elems, 6);
    EXPECT_EQ(1, good.size());

    good = select_sum_vectors(elems, 0);
    EXPECT_EQ(0, good.size());

    good = select_sum_vectors(elems, 1);
    EXPECT_EQ(1, good.size());

    vector< pair<int, int> > v2 = {
        {0,3},
    };
    good = select_sum_vectors(cartesian_product(v2), 1);
    EXPECT_EQ(1, good.size());

    good = select_sum_vectors(cartesian_product(v2), 2);
    EXPECT_EQ(1, good.size());
    
    
}


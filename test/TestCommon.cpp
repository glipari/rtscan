#include "catch.hpp"
#include <vector>
#include <common/generators.hpp>

using namespace std;
using namespace Scan;

TEST_CASE("Generators, Cartesian")
{
    vector< pair<int, int> > v = {
        {1,3},
        {1,3}
    };
    
    vector< vector<int> > elems = cartesian_product(v);
    REQUIRE(9 ==  elems.size());
    pair<int, int> p = {1,1};
    REQUIRE(p.first ==  elems[0][0]);
    REQUIRE(p.second ==  elems[0][1]);
    p = {1,2};
    REQUIRE(p.first ==  elems[1][0]);
    REQUIRE(p.second ==  elems[1][1]);
    p = {1,3};
    REQUIRE(p.first ==  elems[2][0]);
    REQUIRE(p.second ==  elems[2][1]);
    p = {2,1};
    REQUIRE(p.first ==  elems[3][0]);
    REQUIRE(p.second ==  elems[3][1]);

    p = {3,3};
    REQUIRE(p.first ==  elems[8][0]);
    REQUIRE(p.second ==  elems[8][1]);
    
    vector< vector<int> > good = select_sum_vectors(elems, 3);
    REQUIRE(2 ==  good.size());

    good = select_sum_vectors(elems, 7);
    REQUIRE(0 ==  good.size());
}

TEST_CASE("Generators, SelectSum")
{
    vector< pair<int, int> > v1 = {
        {0,3},
        {1,3}
    };
    
    vector< vector<int> > elems = cartesian_product(v1);
    vector< vector<int> > good = select_sum_vectors(elems, 6);
    REQUIRE(1 ==  good.size());

    good = select_sum_vectors(elems, 0);
    REQUIRE(0 ==  good.size());

    good = select_sum_vectors(elems, 1);
    REQUIRE(1 ==  good.size());

    vector< pair<int, int> > v2 = {
        {0,3},
    };
    good = select_sum_vectors(cartesian_product(v2), 1);
    REQUIRE(1 ==  good.size());

    good = select_sum_vectors(cartesian_product(v2), 2);
    REQUIRE(1 ==  good.size());   
}

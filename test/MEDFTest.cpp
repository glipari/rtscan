#include "catch.hpp"

#include <analysis/medf_rta.hpp>

using namespace std;
using namespace Scan;

TEST_CASE("MEDFRTA, Simple1")
{
    vector<Task> tset = {
        Task(5,20,20),
        Task(4,16,16),
        Task(7,28,28), 
        Task(9,36,36)
    };

    REQUIRE(true ==  gedf_iterative(2, tset.begin(), tset.end()));
    REQUIRE(true ==  gedf_bound_gfb(2, tset.begin(), tset.end()));
    REQUIRE(true ==  gfp_bound_density(2, tset.begin(), tset.end()));
}

TEST_CASE("MEDFRTA, Simple2")
{
    vector<Task> tset = {
        Task(4,6,6),
        Task(4,6,6),
        Task(4,6,6)
    };

    REQUIRE(false ==  gedf_iterative(2, tset.begin(), tset.end()));
    REQUIRE(false ==  gedf_bound_gfb(2, tset.begin(), tset.end()));
    REQUIRE(false ==  gfp_bound_density(2, tset.begin(), tset.end()));
}

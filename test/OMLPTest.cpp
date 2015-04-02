#include "catch.hpp"

#include <vector>
#include <algorithm>
#include <models/taskres.hpp>
#include <analysis/omlp.hpp>

using namespace Scan;
using namespace std;

TEST_CASE("OMLPTest, testsmall")
{
    vector<Resource> vres = {
        Resource()
    };
    vector<TaskRes> tset = {
        TaskRes(10,50,50),
        TaskRes(20,100,100),
        TaskRes(30,150,150),
        TaskRes(40,200,200),
    };
    tset[0].addCS(CriticalSection(vres[0].get_id(), 2));
    tset[1].addCS(CriticalSection(vres[0].get_id(), 3));
    tset[2].addCS(CriticalSection(vres[0].get_id(), 5));
    tset[3].addCS(CriticalSection(vres[0].get_id(), 7));

    OMLPAnalysis omlp(3, tset.begin(), tset.end(), vres.begin(), vres.end());
    REQUIRE((7+5+3) ==  omlp.blocking_time_global(tset[0]));
    REQUIRE((7+5+2) ==  omlp.blocking_time_global(tset[1]));
    REQUIRE((7+2+3) ==  omlp.blocking_time_global(tset[2]));
    REQUIRE((2+5+3) ==  omlp.blocking_time_global(tset[3]));

    OMLPAnalysis omlp2(2, tset.begin(), tset.end(), vres.begin(), vres.end());
    REQUIRE((7+7+5) ==  omlp2.blocking_time_global(tset[0]));
    REQUIRE((7+7+5) ==  omlp2.blocking_time_global(tset[1]));
    REQUIRE((7+7+3) ==  omlp2.blocking_time_global(tset[2]));
    REQUIRE((5+5+3) ==  omlp2.blocking_time_global(tset[3]));
}

#include "catch.hpp"

#include <models/task_ex.hpp>
#include <models/has_processor.hpp>

using namespace std;
using namespace Scan;

TEST_CASE("HasProcessor, Simple1")
{
    typedef TaskEx<HasProcessor> TProc;

    TProc t1(10,20,30);
    REQUIRE(-1 ==  t1.get_proc());
    t1.set_proc(2);
    REQUIRE(2 ==  t1.get_proc());
    TProc t2(t1);
    REQUIRE(2 ==  t2.get_proc());
    TProc t3(1, 2, 3);
    REQUIRE(-1 ==  t3.get_proc());
    t3 = t1;
    REQUIRE(2 ==  t3.get_proc());
}


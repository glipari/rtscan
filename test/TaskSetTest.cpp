/*
 * TaskSetTest.cpp
 *
 *  Created on: May 13, 2011
 *      Author: lipari
 */
#include <catch.hpp>
#include <models/taskset.hpp>

using namespace std;
using namespace Scan;

class TaskSetFixture {
public:
    TaskSet tset1;
    TaskSet tset2;
    Task t1;
    Task t2;
    Task t3;
    Task t4;

    TaskSetFixture() : tset1(), tset2(), t1("TA"), t2("TB"), t3("TC"), t4("TD") {
        t1.set_period(10); t1.set_wcet(1); t1.set_dline(10);
        t2.set_period(20); t2.set_wcet(3); t2.set_dline(9);
        t3.set_period(30); t3.set_wcet(5); t3.set_dline(8);
        t4.set_period(40); t4.set_wcet(7); t4.set_dline(7);

        tset1 += t1;
        tset1 += t2;

        tset2 += t3;
        tset2 += t4;
    }
};


TEST_CASE("TaskSetFixture TestAt")
{
    TaskSetFixture f;

    REQUIRE(10 ==  f.tset1.at(0).get_period());
    REQUIRE(20 ==  f.tset1.at(1).get_period());

    REQUIRE(10 ==  f.tset1[0].get_period());
    REQUIRE(20 ==  f.tset1[1].get_period());

    f.tset1[0].set_period(5);

    REQUIRE(5 ==  f.tset1[0].get_period());
    CHECK_THROWS_AS(f.tset1.at(2), IndexOutOfBound);
    CHECK_THROWS_AS(f.tset1[2], IndexOutOfBound);

}

TEST_CASE("TaskSetFixture TestName")
{
    TaskSetFixture f;

    Task *pt1 = &f.tset1[0];
    Task *pt2 = &f.tset1["TA"];
    REQUIRE(pt1 ==  pt2);

    REQUIRE(&f.tset2[0] ==  &f.tset2["TC"]);
}

TEST_CASE("TaskSetFixture TestSum")
{
    TaskSetFixture f;
    TaskSet tset3;

    tset3 = f.tset1 + f.tset2;

    REQUIRE(4 ==  tset3.size());
    CHECK_NOTHROW(tset3["TA"].get_period());
    CHECK_NOTHROW(tset3["TB"].get_period());
    CHECK_NOTHROW(tset3["TC"].get_period());
    CHECK_NOTHROW(tset3["TD"].get_period());
}

TEST_CASE("TaskSetFixture TestSort")
{
    TaskSetFixture f;
    REQUIRE( &f.tset1[0] ==  &f.tset1["TA"] );
    REQUIRE( &f.tset1[1] ==  &f.tset1["TB"] );

    f.tset1.sort<TaskCmpDline>();
    REQUIRE( &f.tset1[0] ==  &f.tset1["TB"] );
    REQUIRE( &f.tset1[1] ==  &f.tset1["TA"] );
    f.tset1.sort<TaskCmpPeriod>();
    REQUIRE( &f.tset1[0] ==  &f.tset1["TA"] );
    REQUIRE( &f.tset1[1] ==  &f.tset1["TB"] );
}

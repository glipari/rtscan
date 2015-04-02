/*
 * DBFTest.cpp
 *
 *  Created on: May 11, 2011
 *      Author: lipari
 */

#include "catch.hpp"

#include <models/task.hpp>
#include <models/taskset.hpp>
#include <analysis/dbf.hpp>

using namespace std;
using namespace Scan;


TEST_CASE("DBFTestFix Single")
{
	Task t1(2.5, 10, 10), t2(1, 6, 12), t3(3, 12, 8);

    DemandBoundFunction d1(t1);

    REQUIRE(10.0 ==  d1.get_interval());
    REQUIRE(10 ==  d1.get_period());

    vector<double> p;
    p.push_back(0);
    p.push_back(10);

    REQUIRE(p ==  d1.get_points());

    p.clear();
    p.push_back(0);
    p.push_back(2.5);

    REQUIRE(p ==  d1.get_values());

    REQUIRE(0.0 ==  d1(8));
    REQUIRE(2.5 ==  d1(12));
    REQUIRE(5.0 ==  d1(25));
    REQUIRE(5.0 ==  d1(28));
    REQUIRE(7.5 ==  d1(32));
}


TEST_CASE("DBFTestFix Single2")
{
	Task t1(2.5, 10, 10), t2(1, 6, 12), t3(3, 12, 8);

    DemandBoundFunction d1(t2);

    REQUIRE(12.0 ==  d1.get_interval());
    REQUIRE(12 ==  d1.get_period());

    vector<double> p;
    p.push_back(0);
    p.push_back(6);

    REQUIRE(p ==  d1.get_points());

    p.clear();
    p.push_back(0);
    p.push_back(1);

    REQUIRE(p ==  d1.get_values());

    REQUIRE(0.0 ==  d1(4));
    REQUIRE(1.0 ==  d1(8));
    REQUIRE(1.0 ==  d1(12));
    REQUIRE(2.0 ==  d1(18));
    REQUIRE(3.0 ==  d1(30));
}

TEST_CASE("DBFTestFix Single3")
{
	Task t1(2.5, 10, 10), t2(1, 6, 12), t3(3, 12, 8);

    DemandBoundFunction d1(t3);

    REQUIRE(12.0 ==  d1.get_interval());
    REQUIRE(8 ==  d1.get_period());

    vector<double> p;
    p.push_back(0);
    p.push_back(12);

    REQUIRE(p ==  d1.get_points());

    p.clear();
    p.push_back(0);
    p.push_back(3);

    REQUIRE(p ==  d1.get_values());

    REQUIRE(0.0 ==  d1(4));
    REQUIRE(0.0 ==  d1(11));
    REQUIRE(3.0 ==  d1(12));
    REQUIRE(3.0 ==  d1(19));
    REQUIRE(6.0 ==  d1(20));
    REQUIRE(6.0 ==  d1(24));
    REQUIRE(9.0 ==  d1(28));
    REQUIRE(9.0 ==  d1(29));
    REQUIRE(12.0 ==  d1(36));
}

TEST_CASE("DBFTestFix Sum")
{
	Task t1(2.5, 10, 10), t2(1, 6, 12), t3(3, 12, 8);

    DemandBoundFunction d1(t1);
    DemandBoundFunction d2(t2);
    DemandBoundFunction d3(t3);

    DemandBoundFunction ds;

    ds = d1 + d2;

    double array[] = {0, 6, 10, 18, 20, 30, 40, 42, 50, 54, 60, 66, 70, 78, 80, 90, 100, 102, 110};
    vector<double> p(array, array+19);

    double values[]= {0, 1, 3.5, 4.5, 7, 10.5, 13, 14, 16.5, 17.5, 20, 21, 23.5, 24.5, 27, 30.5, 33, 34, 36.5};
    vector<double> v(values, values+19);

    REQUIRE(p ==  ds.get_points());
    REQUIRE(v ==  ds.get_values());

    DemandBoundFunction dss = ds + d3;

    double arr2[] = {  0, 6, 10, 12, 18, 20, 28, 30, 36, 40, 42, 44, 50, 52,
                       54, 60, 66, 68, 70, 76, 78, 80, 84, 90, 92, 100, 102,
                       108, 110, 114, 116, 120, 124, 126, 130, 132, 138, 140,
                       148, 150, 156, 160, 162, 164, 170, 172, 174, 180, 186,
                       188, 190, 196, 198, 200, 204, 210, 212, 220, 222, 228,
                       230, 236, 244, 252, 260, 268, 276, 284, 292};

    vector<double> p2(arr2, arr2+69);

    double val2[] = {  0, 1, 3.5, 6.5, 7.5, 13, 16, 19.5, 22.5, 25, 26, 29,
                       31.5, 34.5, 35.5, 41, 42, 45, 47.5, 50.5, 51.5, 54, 57,
                       60.5, 63.5, 69, 70, 73, 75.5, 75.5, 78.5, 78.5, 81.5,
                       82.5, 85, 88, 89, 94.5, 97.5, 101, 104, 106.5, 107.5,
                       110.5, 113, 116, 116, 119, 120, 123, 125.5, 128.5, 129.5,
                       132, 135, 138.5, 141.5, 147, 148, 151, 153.5, 156.5,
                       159.5, 166, 172.5, 175.5, 182, 188.5, 194};

    vector<double> v2(val2, val2+69);

    REQUIRE(p2 ==  dss.get_points());
    REQUIRE(v2 ==  dss.get_values());
}

TEST_CASE("DBFTestFix Sup")
{
	Task t1(2.5, 10, 10), t2(1, 6, 12), t3(3, 12, 8);

    DemandBoundFunction d1(t1);
    DemandBoundFunction d2(t2);
    DemandBoundFunction d3(t3);

    DemandBoundFunction ds;

    ds = sup(d1, d2);

    double array[] = {0, 6, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110};
    vector<double> p(array, array+13);

    double values[]= {0, 1, 2.5, 5, 7.5, 10, 12.5, 15, 17.5, 20, 22.5, 25, 27.5};
    vector<double> v(values, values+13);

    REQUIRE(p ==  ds.get_points());
    REQUIRE(v ==  ds.get_values());
}

TEST_CASE("DBFTestFix TestSumWithNull")
{
	Task t1(2.5, 10, 10), t2(1, 6, 12), t3(3, 12, 8);

	DemandBoundFunction d1(t1);
	DemandBoundFunction d2;
	DemandBoundFunction sum;
	sum = d1 + d2;
	REQUIRE(d1 <= sum);
	REQUIRE(sum <= d1);
}

/*TEST_F(DBFTestFix, FromSet)
{
	TaskSet ts;
	ts += t1;
	ts += t2;
	ts += t3;
	REQUIRE(3 ==  ts.size());

	DemandBoundFunction dbf;
	dbf = ts.get_dbf();

    DemandBoundFunction d1(t1);
    DemandBoundFunction d2(t2);
    DemandBoundFunction d3(t3);
    DemandBoundFunction dtot = d1 + d2 + d3;

    EXPECT_LE(dtot, dbf);
    EXPECT_LE(dbf, dtot);
}

TEST_F(DBFTestFix, PeriodicUtilization)
{
	Task task_1(.1, 10, 10);
	Task task_2(.12, 12, 12);
	Task task_3(.15, 15, 15);
	Task task_4(.18, 18, 18);
	TaskSet tset;
	tset += task_1;   tset += task_2;
	tset += task_3;   tset += task_4;

	double sl = .4;

	while(tset.get_util() < .99) {
		for (unsigned i=0; i<tset.size(); i++) tset[i].set_wcet( tset[i].get_wcet() + .1 );
		DemandBoundFunction dbf = tset.get_dbf();
		dbf.compute_slope(sl, 0);
		EXPECT_NEAR(tset.get_util(), sl, 0.02);
	}
}

*/

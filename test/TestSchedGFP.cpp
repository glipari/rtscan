#include "catch.hpp"

#include <analysis/global_fp.hpp>


using namespace Scan;
using namespace std;

TEST_CASE("Workload and Interference for G-FP")
{
  Task t1 (28, 50, 50);
  Task t2 (13, 30, 30);
  Task t3 (5, 50, 50);
  Task t4 (6, 30, 30);
  Task t5 (6, 40, 40);

  int wcrt1 = 28, wcrt2 = 13, wcrt3 = 18, wcrt4 = 24;

  int x1 = 31, x2 = 38;

  int ncw1 = NCW(t1, x1); // 28
  int ncw2 = NCW(t2, x1); // 14
  int ncw3 = NCW(t3, x1); // 5
  int ncw4 = NCW(t4, x1); // 7

  int ciw1 = CIW(t1, x1, wcrt1); // 28
  int ciw2 = CIW(t2, x1, wcrt2); // 14
  int ciw3 = CIW(t3, x1, wcrt3); // 5
  int ciw4 = CIW(t4, x1, wcrt4); // 12


  REQUIRE(ncw1 == 28);
  REQUIRE(ciw1 == 28);

  REQUIRE(ncw2 == 14);
  REQUIRE(ciw2 == 14);

  REQUIRE(ncw3 == 5);
  REQUIRE(ciw3 == 5);

  REQUIRE(ncw4 == 7);
  REQUIRE(ciw4 == 12);


}

#include "catch.hpp"

#include <analysis/global_edf.hpp>


using namespace Scan;
using namespace std;


/**
 * This file is for testing the implementation of
 * methods for multiprocessor G-FP schedulability analysis.
 */

TEST_CASE("Simple test cases for RTA-LC-EDF algorithms")
{
  Task t1 (28, 50, 50); //0.56
  Task t2 (13, 30, 30); //0.43
  Task t3 (5, 50, 50); //0.10
  Task t4 (6, 30, 30); //0.20
  Task t5 (6, 40, 40); //0.15

  int wcrt1 = 28, wcrt2 = 13, wcrt3 = 18, wcrt4 = 24;

  vector<Task> Ts = {t1, t2, t3, t4, t5};
  REQUIRE (RTA_LC_EDF(Ts, 1, A) == false);
  REQUIRE (RTA_LC_EDF(Ts, 2, A) == true);
  REQUIRE (RTA_LC_EDF(Ts, 3, A) == true);

  //int x1 = 31, x2 = 38;

  //int ncw1 = NCW(t1, x1); // 28
  //int ncw2 = NCW(t2, x1); // 14
  //int ncw3 = NCW(t3, x1); // 5
  //int ncw4 = NCW(t4, x1); // 7

  //int ciw1 = CIW(t1, x1, wcrt1); // 28
  //int ciw2 = CIW(t2, x1, wcrt2); // 14
  //int ciw3 = CIW(t3, x1, wcrt3); // 5
  //int ciw4 = CIW(t4, x1, wcrt4); // 12

  //int nci1 = NCI(t1, t5, x1); 
  //int nci2 = NCI(t2, t5, x1); 
  //int nci3 = NCI(t3, t5, x1); 
  //int nci4 = NCI(t4, t5, x1); 

  //int cii1 = CII(t1, t5, x1, wcrt1); 
  //int cii2 = CII(t2, t5, x1, wcrt2); 
  //int cii3 = CII(t3, t5, x1, wcrt3); 
  //int cii4 = CII(t4, t5, x1, wcrt4); 



  //REQUIRE(ncw1 == 28);
  //REQUIRE(ciw1 == 28);

  //REQUIRE(ncw2 == 14);
  //REQUIRE(ciw2 == 14);

  //REQUIRE(ncw3 == 5);
  //REQUIRE(ciw3 == 5);

  //REQUIRE(ncw4 == 7);
  //REQUIRE(ciw4 == 12);


  //REQUIRE(nci1 == 26);
  //REQUIRE(nci2 == 14);
  //REQUIRE(nci3 == 5);
  //REQUIRE(nci4 == 7);

  //REQUIRE(cii1 == 26);
  //REQUIRE(cii2 == 14);
  //REQUIRE(cii3 == 5);
  //REQUIRE(cii4 == 12);

  //vector<Task> tasks = {t1, t2, t3, t4, t5};
  //REQUIRE(RTA_CE(tasks, 2) == false);

  //vector<Task> hps;
  //vector<int> wcrts;

  ////int wcrt1 = 28, wcrt2 = 13, wcrt3 = 18, wcrt4 = 24;
  //REQUIRE ( RTA_CE(t1, hps, wcrts, 2) == wcrt1); 
  //hps.push_back(t1);
  //wcrts.push_back(wcrt1);

  //REQUIRE ( RTA_CE(t2, hps, wcrts, 2) == wcrt2); 
  //hps.push_back(t2);
  //wcrts.push_back(wcrt2);

  //REQUIRE ( RTA_CE(t3, hps, wcrts, 2) == wcrt3); 
  //hps.push_back(t3);
  //wcrts.push_back(wcrt3);

  //REQUIRE ( RTA_CE(t4, hps, wcrts, 2) == wcrt4); 
  //hps.push_back(t4);
  //wcrts.push_back(wcrt4);

  //REQUIRE ( RTA_CE(t5, hps, wcrts, 2) > t5.get_dline()); 
  //cout << RTA_CE(t5, hps, wcrts, 2) << endl;


  //vector<int> v = {2, 3, 4, 0, 6, 1, 5};
  //sort(v.begin(), v.end(), [](const int a, const int b){return a > b;});
  //REQUIRE(v[0] == 6);
  //REQUIRE(v[1] == 5);
  //REQUIRE(v[2] == 4);
  //REQUIRE(v[3] == 3);
  //REQUIRE(v[4] == 2);
  //REQUIRE(v[5] == 1);
  //REQUIRE(v[6] == 0);

}

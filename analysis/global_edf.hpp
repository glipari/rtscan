#ifndef __GLOBAL_EDF_HPP__
#define __GLOBAL_EDF_HPP__

#include <vector>
#include <cmath>
#include <ostream>

#include <models/task.hpp>


namespace Scan {


  /**
   *  Below is the implementation of the RTA with LC for G-EDF scheduling.
   *  The original algorithms are proposed in 
   *  "Response Time Analysis with Limited Carry-in for Global Earliest Deadline First Scheduling"
   *  by Youcheng Sun and Giuseppe Lipari @ RTSS2015
   *
   */

  /**
   * Compute the DBF of a task "t" with respect to time interval length l
   */
  int DBF(const Task& t, const int l);

  /**
   * To upper bound the workload by a NC task ti in the sub window that may
   * interfere the target task tk's execution.
   */
  int NCW(const Task& ti, const Task& tk, const int x, const int L); 

  /**
   * To upper bound the interference by a NC task ti in the sub window
   * upon the target task tk's execution.
   */
  int NCI(const Task& ti, const Task& tk, const int x, const int L); 

  /**
   * To upper bound the workload by a CI task ti in the sub window that may
   * interfere the target task tk's execution.
   * wcrt is the WCRT of ti.
   */
  int CIW(const Task& ti, const int wcrt, const Task& tk, const int x, const int L); 

  /**
   * To upper bound the interference by a CI task ti in the sub window
   * upon the target task tk's execution.
   */
  int CII(const Task& ti, const int wcrt, const Task& tk, const int x, const int L); 


  /**
   * It is not necessarily to check every Ak value; instead, only these Ak values
   * with which the DBF of some task changes are really interesting.
   */
  bool DBF_change(const std::vector<Task>& Ts, const int L);

  /**
   * To test if "Ak" is a valid busy period length
   */
  bool is_valid(const Task& tk, const std::vector<Task>& Ts, const std::vector<int>& wcrts, const int m, const int Ak, const bool ceiling = false);

  /**
   *  The alpha bound on the busy period length Ak values.
   *  tk: the target task
   *  Ts: the task set containing tk
   *  m: the number of processors
   */
  double Ak_alpha(const Task& tk, const std::vector<Task>& Ts, const int &m);


  /**
   *  The beta bound on the busy period length Ak values.
   *  tk: the target task
   *  Ts: the task set containing tk
   *  m: the number of processors
   */
  double Ak_beta(const Task& tk, const std::vector<Task>& Ts, const int &m);

  /**
   * A denotes the test RTA-LC-EDF
   * and B denotes its faster variant, i.e. RTA-LC-EDF-B
   * By default, the RTA-LC-EDF for schedulability check is triggered.
   */
  enum SCHED_TYPE {A, B};
  bool RTA_LC_EDF(const std::vector<Task> &Ts, const int m, const enum SCHED_TYPE st = A, const bool ceiling = false);
  
  /**
   * To compute the WCRT of the target task tk
   */
  int RTA_LC_EDF_WCRT(const Task& tk, const std::vector<Task>& Ts, std::vector<int>& wcrts, const int m, const bool ceiling = false);

  /**
   * A faster approach for computing WCRT upper bound in RTA-LC-EDF-B.
   */
  int RTA_LC_EDF_B_WCRT(const Task& tk, const std::vector<Task>& Ts, std::vector<int>& wcrts, const int m, const bool ceiling = false);

}

#endif

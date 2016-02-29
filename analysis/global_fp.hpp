#ifndef __GLOBAL_FP_HPP__
#define __GLOBAL_FP_HPP__

#include <vector>
#include <cmath>
#include <ostream>
#include <map>

#include <models/task.hpp>


namespace Scan {

  /**
   * This file implements a series of methodologies for 
   * schedulability analysis of multiprocessor Global EDF 
   * scheduling.
   */

  /**
   * Compute the NC workload of a task tau_i in a time 
   * interval with length "x".
   */ 
  int NCW(const Task& t_i, const int x); 

  /**
   * Compute the CI workload of a task tau_i in a time 
   * interval with length "x", and "wcrt_i" is the 
   * worst-case response time of task tau_i.
   *
   * References:
   * Youcheng Sun, Giuseppe Lipari, Nan Guan and Wang Yi.
   * "Improving the Response Time Analysis of Global 
   * Fixed-Priority Multiprocessor Scheduling".
   */ 
  int CIW(const Task& t_i, const int x, const int wcrt_i); 


  /**
   * Compute the NC interference of a task tau_i over the 
   * target task tau_k within a time interval with length "x".
   */ 
  int NCI(const Task& t_i, const Task& t_k, const int x); 

  /**
   * Compute the CI interference of a task tau_i over the 
   * target task tau_k within a time interval with length "x",
   * and "wcrt_i" is the worst-case response time of tau_i.
   */ 
  int CII(const Task& t_i, const Task& t_k, const int x, const int wcrt_i); 

  /**
   * Nan Guan, Martin Stigge, Wang Yi and Ge  Yu.
   * "New Response Time Bounds for Fixed Priority
   * Multiprocessor Scheduling".
   *
   * Youcheng Sun. PhD thesis: "Real-Time Schedulability
   * Analysis with Formal Techniques".
   *
   *
   * Note that different from the original algorithm, here we refer to
   * continuous-time, instead of discrete-time, schedule.
   *
   */
  bool RTA_LC (const std::vector<Task>& tasks, const int m);

  /**
   * Given the set of higher priority tasks within "hps",
   * to compute the WCRT of the target task "tk". The 
   * WCRTs of all higher priority tasks are stored in
   * the vector "wcrts".
   */
  int RTA_LC (const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m);



  /**
   * Schedulability Analysis by exploring the Parallel Execution of 
   * different tasks (work submitted to ECRTS'16).
   */
  bool baseline (const std::vector<Task>& tasks, const int m);

  int baseline_wcrt (const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m);


  int FIk (const int ck1, const int ck2, const int L1star, const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m);

}

#endif

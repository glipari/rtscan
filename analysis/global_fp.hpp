#ifndef __GLOBAL_FP_HPP__
#define __GLOBAL_FP_HPP__

#include <vector>
#include <cmath>
#include <ostream>

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
   */
  bool RTA_CE (const std::vector<Task>& tasks, const int m, const bool ceiling = false);

  /**
   * Given the set of higher priority tasks within "hps",
   * to compute the WCRT of the target task "tk". The 
   * WCRTs of all higher priority tasks are stored in
   * the vector "wcrts".
   */
  int RTA_CE (const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m, const bool ceiling = false);
}

#endif

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


}

#endif

#include "global_fp.hpp"
#include <algorithm>

namespace Scan {

  using namespace std;

  int NCW(const Task& t_i, const int x) {
    int p = t_i.get_period();
    int c = t_i.get_wcet();

    int n = x / p;
    int n_ = x % p;

    return n * c + fmin(c, n_);
  }

  int CIW(const Task& t_i, const int x, const int wcrt_i) {

    int p = t_i.get_period();
    int c = t_i.get_wcet();

    int xp = c + p - wcrt_i;

    int w = NCW(t_i, fmax(x-xp, 0));

    w += fmin(c, x);

    return w;
  }

  int NCI(const Task& t_i, const Task& t_k, const int x) {
    
    int w = NCW(t_i, x);

    int c_k = t_k.get_wcet();

    return fmin(w, x-c_k+1);
  }

  int CII(const Task& t_i, const Task& t_k, const int x, const int wcrt_i) {
    
    int w = CIW(t_i, x, wcrt_i);

    int c_k = t_k.get_wcet();
    
    return fmin(w, x-c_k+1);
  }


  bool RTA_CE (const std::vector<Task>& tasks, const int m, const bool ceiling) {

    vector<int> wcrts;
    vector<Task> hps; // the set of higher priority tasks when analysing the target one

    for ( auto it = tasks.begin(); it != tasks.end(); it++) {

      int wcrt = RTA_CE(*it, hps, wcrts, m, ceiling);
      if ( wcrt > it->get_dline()) return false;
      wcrts.push_back(wcrt);
      hps.push_back(*it);
    }

    return true;
  }

  int RTA_CE (const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m, const bool ceiling) {

    if ( hps.size() < m) return tk.get_wcet();

    int L = tk.get_wcet();

    while (true) {

      vector<int> ncis, diffs;
      int i = 0, totI = 0;

      for ( auto & x : hps) {
        int nci = NCI(x, tk, L);
        int cii = CII(x, tk, L, wcrts[i++]);

        totI += nci;
        diffs.push_back(cii-nci);
      }

      sort(diffs.begin(), diffs.end(), 
          [](const int a, const int b) {return a > b;}
          );

      for ( i = 0; i < m-1; i++) // at most m-1 CI tasks
        totI += diffs[i];

      int Y;
      if ( not ceiling) Y = totI / m + tk.get_wcet();
      else Y = ceil( totI*1.0 / m) + tk.get_wcet();
      
      if ( Y <= L or Y > tk.get_dline()) return Y;
      L = Y;
      
    }

  }

}


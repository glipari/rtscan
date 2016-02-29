#include "global_fp.hpp"
#include <algorithm>
#include <climits>

namespace Scan {

  static bool debug = false;
  static int seed = 0;
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

    return fmin(w, x-c_k);
    //return fmin(w, x-c_k+1);
  }

  int CII(const Task& t_i, const Task& t_k, const int x, const int wcrt_i) {
    
    int w = CIW(t_i, x, wcrt_i);

    int c_k = t_k.get_wcet();
    
    return fmin(w, x-c_k);
    //return fmin(w, x-c_k+1);
  }



  bool RTA_LC (const std::vector<Task>& tasks, const int m) {

    vector<int> wcrts;
    vector<Task> hps; // the set of higher priority tasks when analysing the target one

    for ( auto it = tasks.begin(); it != tasks.end(); it++) {

      int wcrt = RTA_LC(*it, hps, wcrts, m);
      if ( wcrt > it->get_dline()) return false;
      wcrts.push_back(wcrt);
      hps.push_back(*it);
    }

    return true;
  }

  int RTA_LC (const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m) {

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

      if ( totI < m*(L-tk.get_wcet())) return L;
      int Y = ceil( totI*1.0 / m) + tk.get_wcet();
      
      if ( Y > L) L = Y;
      else L ++;

      if ( L > tk.get_dline()) return L;
      
    }

  }


  /****** Exploring the parallel execution of different tasks for multiprocessor global schedulability analysis. ***********/


  bool baseline (const std::vector<Task>& tasks, const int m) {

    vector<int> wcrts;
    vector<Task> hps; // the set of higher priority tasks when analysing the target one

    for ( auto it = tasks.begin(); it != tasks.end(); it++) {

      int wcrt = baseline_wcrt(*it, hps, wcrts, m);
      if ( wcrt > it->get_dline()) return false;
      wcrts.push_back(wcrt);
      hps.push_back(*it);
    }

    return true;
  }

  int baseline_wcrt (const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m) {

    int ck = tk.get_wcet();
    
    if ( hps.size() < m) return ck;

    int dk = tk.get_dline();
    int wcrt = dk + 1;

    for ( int ck1 = 0; ck1 <= ck; ck1 ++) {

      int ck2 = ck - ck1; // for each pair of (ck1, ck2)

      // To compute the upper bound on the 1st level response time
      Task copy_tk(tk);
      copy_tk.set_wcet(ck1);

      int Rk1 = RTA_LC(copy_tk, hps, wcrts, m);
      

      if ( Rk1 + ck2 > dk) break;

      int res = FIk(ck1, ck2, Rk1, tk, hps, wcrts, m);
      if ( res < wcrt) wcrt = res;
      
    }

    return wcrt;

  }


  /** The latest version of the FIk function */

  int FIk (const int CK1, const int CK2, const int L1star, const Task& tk, const std::vector<Task>& hps, const std::vector<int>& wcrts, const int m) {

    if ( hps.size() < m) return L1star + CK2;
    
    int dk = tk.get_dline();
    
    int l = L1star + CK2; //, L1 = CK1;
    while (true) {

      if ( l > dk) return dk + 1;  
      bool sched = true; 
      
      const int l1 = L1star; 
      int ck1 = CK1, ck2 = CK2;
      while (true) {

        if ( ck1 > l1 ) break;
        int l2 = l - l1; 
        
        // The early termination condition
        vector<int> diffs;
        int totI2 = 0;
        int totI1 = 0;
        Task cpk2(tk); cpk2.set_wcet(ck2);

        for ( int i = 0; i < hps.size(); i++) {
          auto &x = hps[i];
          int ci = CII(x, cpk2, l2, wcrts[i]);
          int nc = NCI(x, cpk2, l2);
          totI1 += nc;
          diffs.push_back(ci-nc);
        }
        sort(diffs.begin(), diffs.end(), 
            [](const int a, const int b) {return a > b;}
            );
        for ( int i = 0; i < m-1; i++) {// at most m-1 CI tasks
          totI1 += diffs[i];
        }


        // The total interference the target job of tau_k suffers
        diffs.clear();
        for ( int i = 0; i < hps.size(); i++) {
          auto &x = hps[i];
          int ciw = CII( x, tk, l, wcrts[i]);
          int ncw = NCI(x, tk, l);

          // a) if "x" is a CI task for l2
          int cii2 = CII(x, cpk2, l2, wcrts[i]); 

          ///// 1) if "x" is a NC task for l1 : ncci
          int nci1 = NCW(x, l1);

          if ( nci1 > l1 - ck1) {
            nci1 = l1-ck1;
          }
          int ncci = fmin(cii2, ncw - nci1 ) + nci1;

          ///// 2) if "x" is a CI task for l1 : cici
          int cii1 = CIW(x, l1, wcrts[i]); 
          if ( cii1 > l1 - ck1) {
            cii1 = l1 - ck1;
          }
          int cici = fmin(cii2, ciw - cii1) + cii1;

          totI2 += ncci;
          diffs.push_back(cici-ncci);

        }

        sort(diffs.begin(), diffs.end(), 
            [](const int a, const int b) {return a > b;}
            );
        
        for ( int i = 0; i < m-1; i++) {// at most m-1 CI tasks
          totI2 += diffs[i];
        }


        if( totI1 < m*(l2-ck2)) break;

        if ( totI2 >= m*(l-ck1-ck2)) {
          int lnu = ceil (1.0* totI2 / m) + ck1 + ck2;
          if ( lnu > l) l = lnu;
          else l ++;
          sched = false; break;
        }
        else {ck1 ++; ck2 --; if (ck1 > CK1+CK2) break;}

      }
      
      if (sched) {
        return l;
      }
    
    }
    
  }


}


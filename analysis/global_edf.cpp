#include "global_edf.hpp"
#include <algorithm>

using namespace std;

namespace Scan {

  int DBF( const Task &t, const int l) {
    int d = t.get_dline();
    int p = t.get_period();
    int c = t.get_wcet();
    if ( l < d) return 0;
    return ( (l-d)/p + 1) * c;
  }

  int NCW(const Task& ti, const Task& tk, const int x, const int L) {
    int W = 0, p =0;
    int di = ti.get_dline();
    int ci = ti.get_wcet();
    int pi = ti.get_period();
    while ( p < x) {
      if ( p + di <= L) {
        W += fmin(x-p, ci);
        p += pi;
      } 
      else break;
    }
    if ( ti.get_id() == tk.get_id()) {
      int s = L - pi;
      int w2 = DBF(ti, s);
      if ( W > w2)
        W = w2;
    }
    return W;
  }

  int NCI(const Task& ti, const Task& tk, const int x, const int L) {
    int workload = NCW(ti, tk, x, L);
    return fmin( workload, L - tk.get_wcet() +1);
  }

  int CIW(const Task& ti, const int wcrt, const Task& tk, const int x, const int L) {
    int W = 0;
    int di = ti.get_dline();
    int pi = ti.get_period();
    int ci = ti.get_wcet();
    int ri = fmin(wcrt, di);

    int p = fmin(x-ci, L-di);
    if ( p < 0) {
      W = fmin( L-(di-ri), ci);
      if( W < 0) W = 0;
      if( W > x) W = x;
    }
    else {
      W = ci;
      int N = p / pi;
      W += N * ci;
      W += fmin( ci, fmax(p % pi - (pi-ri), 0));
    }

    if ( tk.get_id() == ti.get_id()) {
      int t = L-pi;
      int w2 = t/pi * ci + fmin(ci, fmax(t % pi - di + ri, 0));

      if ( W > w2) 
        W = w2;
    }

    return W;
  }

  int CII(const Task& ti, const int wcrt, const Task& tk, const int x, const int L) {
    int workload = CIW(ti, wcrt, tk, x, L);
    return fmin( workload, L - tk.get_wcet() +1);
  }

  bool DBF_change(const std::vector<Task>& Ts, const int L) {
    for ( auto &x :Ts) 
      if ( DBF(x, L - 1) != DBF(x, L)) return true;
    return false;
  }

  bool is_valid(const Task& tk, const std::vector<Task>& Ts, const std::vector<int>& wcrts, const int m, const int Ak, bool ceiling) {
    int x = 1; 
    int dk = tk.get_dline();
    while (true) {
      int W = 0;
      vector<int> ncws, ciws, diffs;
      for ( int i = 0; i < Ts.size(); i++) {
        int w1 = NCW(Ts[i], tk, x, dk+Ak);
        ncws.push_back(w1);
        W += w1;

        int w2 = CIW(Ts[i], wcrts[i], tk, x, dk+Ak);
        ciws.push_back(w2);
        diffs.push_back(w2 - w1);
      }
      
      sort(diffs.begin(), diffs.end(), 
        [](const int a, const int b) {return a > b;}
        );

      for ( int i = 0; i < m-1; i++)
        W += diffs[i];

      int nuX;
      if ( ceiling) nuX = ceil( (1.0*W) / m);
      else nuX = W / m;

      if ( nuX > Ak) return true;
      else if ( nuX == x) return false;

      x = nuX;
    }

  }

  double Ak_alpha(const Task& tk, const std::vector<Task>& Ts, const int &m) {
    
    double up = 0, down = 0, totU = 0;
    
    vector<int> Cs;
    for (auto &x : Ts) {
      int p = x.get_period();
      int c = x.get_wcet();

      up += (p-c)*( (1.0*c)/p );
      totU += (1.0*c) / p;
      Cs.push_back(c);
    }
    
    sort(Cs.begin(), Cs.end(), 
        [](const int a, const int b) {return a > b;}
        );

    for (int i = 1; i < m-1; i++)
      up += Cs[i-1];

    down += m - totU;

    return up / down;

  }

  double Ak_beta(const Task& tk, const std::vector<Task>& Ts, const int &m) {
    double up = 0, down = 0, totU = 0;

    vector<int> Cs;
    for ( auto &x : Ts) Cs.push_back(x.get_wcet());

    sort(Cs.begin(), Cs.end(), 
        [](const int a, const int b) {return a > b;}
        );

    for (int i = 1; i < m-1; i++)
      up += Cs[i-1];

    for ( auto & x : Ts) {
      int p = x.get_period();
      int d = x.get_dline();
      int c = x.get_wcet();
      up += (p-d) * ( (1.0*c)/p );
      totU += (1.0*c) / p;
    }

    up += tk.get_dline() * (totU - (1.0*tk.get_wcet())/tk.get_period());

    down = m - totU;

    return up / down;

  }

  bool RTA_LC_EDF(const std::vector<Task> &Ts, const int m, const enum SCHED_TYPE st, const bool ceiling) {
    /**
     * We only consider the case such that total task set utilisation < m.
     */
    bool totU = 0;
    for ( auto & x : Ts) totU += (1.0*x.get_wcet()) / x.get_period();
    if (totU >= m ) return false;

    /**
     * The WCRT of a task is initialised to be "dline+1"
     */
    vector<int> wcrts;
    for ( auto &x: Ts)
      wcrts.push_back(x.get_dline() + 1); 


    while (true) {
      /**
       * The "schedulable" flag is initialised to be true and every time
       * a deadline miss task is detected, it is set to be false;
       */
      bool schedulable = true;

      /**
       * The "slack_changing" flag tracks if the response time of some task
       * is updated. If no slack change happens among all tasks, then the 
       * test can stop.
       */
      bool slack_changing = false;

      //vector<int> slacks;
      //for ( int i = 0; i < Ts.size(); i++) 
      //  slacks.push_back( Ts[i].get_dline() - slacks[i]);

      for ( int i = 0; i < Ts.size(); i++) {
        auto & tk = Ts[i];

        int wcrt;
        if (st == A) wcrt = RTA_LC_EDF_WCRT(tk, Ts, wcrts, m, ceiling);
        else wcrt = RTA_LC_EDF_B_WCRT(tk, Ts, wcrts, m, ceiling);

        if ( wcrt < tk.get_dline()) {
          if ( wcrts[i] > wcrt) {
            wcrts[i] = wcrt;
            slack_changing = true;
          }
          if ( wcrts[i] < wcrt) 
            throw string("the wcrt of a task is increased after an iteration step in RTA-LC-EDF");
        }
        else 
          schedulable = false;
      }

      if ( schedulable) return true;
      else if (not slack_changing) return false;

    }

  }

  int RTA_LC_EDF_WCRT(const Task& tk, const std::vector<Task>& Ts, std::vector<int>& wcrts, const int m, const bool ceiling) {

    int wcrt = tk.get_wcet();
    int dk = tk.get_dline();
    int ck = tk.get_wcet();

    double Ak_max = fmin( Ak_alpha(tk, Ts, m), Ak_beta(tk, Ts, m));

    /**
     * For each Ak value, compute the corresponding response time
     * upper bound of tk. In the end, the largest one will be returned.
     */
    for ( int Ak = 0; Ak < Ak_max; Ak ++) {

      if ( not is_valid(tk, Ts, wcrts, m, Ak, ceiling)) continue;
      if (Ak != 0 and not DBF_change(Ts, Ak+dk)) continue;

      int x = Ak + ck;
      while (true) {
        // To compute Omega1, according to the LC assumption
        int Omega1 = 0;
        vector<int> ncis, ciis, diffs;
        for ( int i = 0; i < Ts.size(); i++) {
          int i1 = NCI(Ts[i], tk, x, dk+Ak);
          ncis.push_back(i1);
          Omega1 += i1;

          int i2 = CII(Ts[i], wcrts[i], tk, x, dk+Ak);
          ciis.push_back(i2);
          diffs.push_back(i2 - i1);
        }
      
        sort(diffs.begin(), diffs.end(), 
          [](const int a, const int b) {return a > b;}
          );

        for ( int i = 0; i < m-1; i++)
          Omega1 += diffs[i];

        // to compute Omega2
        int Omega2 = m * Ak;
        for ( int i = 0; i < Ts.size(); i++) {
          if ( Ts[i].get_id() == tk.get_id()) continue;
          Omega2 += CII(Ts[i], wcrts[i], tk, x-Ak, dk);
        }
      
        // In the end, the smaller one between Omega1 and Omega2 will
        // be the final upper bound on the tital Interference within
        // the sub window
        int Omega = fmin(Omega1, Omega2);

        int nuX;
        if (ceiling) nuX = ceil( (1.0*Omega) / m) + ck;
        else nuX = Omega / m + ck;
        if ( nuX > dk + Ak) return dk + 1;
        else if (nuX == x) break;
        x = nuX;
      }
      
      if( wcrt < x - Ak) wcrt = x - Ak;

    }
    
    return wcrt;

  }

  int RTA_LC_EDF_B_WCRT(const Task& tk, const std::vector<Task>& Ts, std::vector<int>& wcrts, const int m, const bool ceiling) {

    int dk = tk.get_dline();
    int ck = tk.get_wcet();

    double Ak_max = fmin( Ak_alpha(tk, Ts, m), Ak_beta(tk, Ts, m));
    int Aky = -1;

    int y = ck;
    while (true) {

      int ftime = 0; // with each y, we would like to get the largest finishing tim (ftime) among all possible Ak values
      for ( int Ak = 0; Ak < Ak_max; Ak++) {

        if ( Ak < Aky) continue;
        if (Ak != 0 and not DBF_change(Ts, Ak+dk)) continue;
        
        int Omega1 = 0;
        vector<int> ncis, ciis, diffs;
        for ( int i = 0; i < Ts.size(); i++) {
          // The sub window = Ak-window + y-window
          int i1 = NCI(Ts[i], tk, Ak+y, dk+Ak);
          ncis.push_back(i1);
          Omega1 += i1;
          
          int i2 = CII(Ts[i], wcrts[i], tk, Ak+y, dk+Ak);
          ciis.push_back(i2);
          diffs.push_back(i2 - i1);
        }
        
        sort(diffs.begin(), diffs.end(), 
            [](const int a, const int b) {return a > b;}
            );

        for ( int i = 0; i < m-1; i++)
          Omega1 += diffs[i];

        Omega1 -= m*Ak; // this measures the total interference in the y-window

        // to compute Omega2
        int Omega2 = 0;
        for ( int i = 0; i < Ts.size(); i++) {
          if ( Ts[i].get_id() == tk.get_id()) continue;
          Omega2 += CII(Ts[i], wcrts[i], tk, y, dk);
        }
        
        // In the end, the smaller one between Omega1 and Omega2 will
        // be the final upper bound on the tital Interference within
        // the sub window
        int Omega = fmin(Omega1, Omega2);

        int nu_ftime;
        if (ceiling) nu_ftime =  ceil( (1.0*Omega) / m) + ck;
        else nu_ftime = Omega / m + ck;

        if ( nu_ftime > ftime) ftime = nu_ftime;
        if ( ftime > y) {Aky = Ak; break;}

      }

      if (ftime > dk) return dk + 1;
      else if (ftime == y) return y;
      y = ftime;
    }
  }
} 

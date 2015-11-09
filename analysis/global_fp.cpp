#include "global_fp.hpp"

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



}


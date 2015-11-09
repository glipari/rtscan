#include "global_fp.hpp"

namespace Scan {

  using namespace std;

  int NCW(const Task& t_i, const int x) {
    int p = t_i.get_period();
    int c = t_i.get_wcet();

    int n = p / x;
    int n_ = p % x;

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




}


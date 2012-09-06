#include <vector>
#include <cmath>

typedef std::vector<double> row_t;
typedef std::pair<row_t, double> constraint_t;
typedef std::vector<constraint_t> space_t;

inline double coeff(double t, int p)
{
    return std::ceil(t/p);
}

template <class Iter>
std::vector<double> compute_points(Iter b, Iter e, double d)
{
    std::vector<double> points;
    // first try: all points
    for (Iter i=b; i !=e; i++) {
        int k=1; 
        while (k * i->get_period() < d) {
            points.push_back(k * i->get_period()); k++;
        }
    }
    points.push_back(d);
    return points;
}

template <class Iter>
space_t compute_space(Iter b, Iter e)
{
    Iter ct = e-1;
    double dline = ct->get_dline();
    std::vector<double> points = compute_points(b, ct, dline); 
}

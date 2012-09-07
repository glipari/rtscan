#include <vector>
#include <cmath>
#include <ostream>

namespace Scan {

    typedef std::vector<double> point_t;
    typedef std::vector<double> coeff_row_t;

    struct constraint_t {
        coeff_row_t a;
        int sign;  // lt : -1; eq : 0; gt : 1
        double b;
    };   

    ostream& operator<<(ostream &os, const constraint_t &s);
    
    class constraint_set_t {
    protected:
        std::vector<constraint_t> cs;
        virtual bool is_in(const point_t &p) const = 0;
    public:
        bool contains(const point_t &p) const;
        void add_constraint(const constraint_t &c);
        constraint_t &operator[](unsigned index);
    };    
    
    class conjunct_space_t : public constraint_set_t {
    protected:
        bool is_in(const point_t &p) const;
    };

    class disjunct_space_t : public constraint_set_t {
    protected:
        bool is_in(const point_t &p) const;
    };
    
    class space_t : public constraint_set_t {
    protected:
        std::vector<constraint_set_t>;
        bool is_in(const point_t &p) const;
    public:
        void add_constraint_set(const constraint_set_t &s);
    };
            
    // Ax >= 0
    conjunct_space_t positive_space(int n)
    {
        conjunct_space_t res;
        for (int i=0; i<n; ++i) {
            coeff_row_t r;
            for (int j=0; j<n; ++j) r.push_back(0);
            r[i] = 1;
            constraint_t c = {r, 1, 0};
            res.push_back(c);
        }
        return res;
    }
    
    space_t conjunction(const space_t &a, const space_t &b)
    {
        space_t result = a;
        for (auto x : b) {
            result.push_back(x);
        }
        return result;
    }


    inline double compute_coeff(double t, int p)
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

    // template <class Iter>
    // space_t compute_space(Iter b, Iter e)
    // {
    //     Iter ct = e-1;
    //     double dline = ct->get_dline();
    //     std::vector<double> points = compute_points(b, ct, dline); 
    // }
}

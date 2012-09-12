#include <cassert>
#include <iostream>
#include "constraint_space.hpp"

namespace Scan {

    const int plane_t::lt = -2;
    const int plane_t::lte = -1;
    const int plane_t::gt = 2;
    const int plane_t::gte = 1;
    const int plane_t::eq = 0;
    
    constraint_t::constraint_t(size_t n) : num_vars(n) {}

    bool constraint_t::contains(const point_t &p) const
    {
        return is_in(p);
    }

    constraint_t *constraint_t::complement() const
    {
        return negate();
    }


    plane_t::plane_t(size_t n) : constraint_t(n), a(n), sign(0), b(0)
    {
        for (auto &x : a) x = 0; 
    }
    
    plane_t::plane_t(const coeff_row_t &coeff, int s, double c) :
        constraint_t(coeff.size()), a(coeff), sign(s), b(c)
    {
    }

    plane_t * plane_t::copy() const
    {
        return new plane_t(*this);
    }
    
    std::ostream& operator<<(std::ostream &os, const plane_t &s)
    {
        int i = 1;
        for (auto x : s.a) {
            if (x > 0) 
                os << " + " << x << " x(" << i << ")";
            else if (x < 0) 
                os << " " << x << " x(" << i << ")";
            else os << " + 0 "; 
            i++;
        } 
        if (s.sign == plane_t::lt)      os << " <  ";
        else if (s.sign == plane_t::lte) os << " <= ";
        else if (s.sign == plane_t::eq)  os << " =  ";
        else if (s.sign == plane_t::gte)  os << " >= ";
        else if (s.sign == plane_t::gt)  os << " >  ";
    
        os << s.b;
        return os;
    }
    
    bool plane_t::is_in(const point_t &p) const
    {
        assert(p.size() == a.size());
        double sum = 0;

        for (unsigned i=0; i<a.size(); ++i)
            sum += a[i] * p[i];

        //std::cerr << "Sum = " << sum << std::endl;

        switch(sign) {
        case lt: return (sum < b);
            break;
        case lte: return (sum <= b);
            break;
        case eq: return (sum == b);
            break;
        case gt: return (sum > b);
            break;
        case gte: return (sum >= b);
            break;
        }
        assert(false);
        return false;
    }

    void plane_t::change_sign()
    {
        for (auto &x : a) x = -x;
        sign = -sign;
        b = -b;
    }

    plane_t plane_t::normal_form() const
    {
        plane_t x = *this;
        if (x.sign == gt || x.sign == gte) x.change_sign();
        return x;
    }
        

    plane_t *plane_t::negate() const
    {
        plane_t *p = copy();
        p->sign = -sign;
        return p;
    }

    space_t::space_t(size_t n) : constraint_t(n), cs()
    {
    }

    space_t::~space_t() 
    {
        for (auto x : cs) delete x;
    }

    space_t::space_t(const space_t &s) : constraint_t(s)
    {
        for (auto x : s.cs) 
            cs.push_back(x->copy());
    }
    
    void space_t::add_constraint(const constraint_t &c)
    {
        assert(c.get_nvars() == num_vars);
        constraint_t *p = c.copy();
        cs.push_back(p);
    }

    void space_t::add_constraint(constraint_t *c)
    {
        assert(c->get_nvars() == num_vars);
        cs.push_back(c);
    }

    size_t space_t::size() const
    {
        return cs.size();
    }

    conjunct_space_t::conjunct_space_t(size_t n) : space_t(n)
    {
    }


    constraint_t *space_t::get(unsigned r) 
    {
        assert(r < cs.size());
        
        return cs.at(r);
    }

    conjunct_space_t * conjunct_space_t::copy() const
    {
        conjunct_space_t *p = new conjunct_space_t(*this);
        return p;
    }
    
    
    bool conjunct_space_t::is_in(const point_t &p) const
    {
        for (auto x : cs) 
            if (!x->contains(p)) return false;
        
        return true;
    }
    
    space_t * conjunct_space_t::negate() const
    {
        disjunct_space_t *space = new disjunct_space_t(num_vars);
        for (auto c : cs) 
            space->add_constraint(c->negate());
        return space;
    }


    disjunct_space_t::disjunct_space_t(size_t n) : space_t(n)
    {
    }

    disjunct_space_t * disjunct_space_t::copy() const
    {
        disjunct_space_t *p = new disjunct_space_t(*this);
        return p;
    }
    

    bool disjunct_space_t::is_in(const point_t &p) const
    {
        for (auto x : cs) 
            if (x->contains(p)) return true;
        
        return false;
    }

    space_t * disjunct_space_t::negate() const
    {
        conjunct_space_t *space = new conjunct_space_t(num_vars);
        for (auto c : cs) 
            space->add_constraint(c->negate());
        return space;
    }

    conjunct_space_t non_negative_space(int n)
    {
        conjunct_space_t res(n);
        for (int i=0; i<n; ++i) {
            coeff_row_t r;
            for (int j=0; j<n; ++j) r.push_back(0);
            r[i] = 1;
            plane_t c = {r, plane_t::gte, 0};
            res.add_constraint(c);
        }
        return res;
    }

    std::vector<plane_t> get_normalized_planes(conjunct_space_t &space)
    {
        int n = space.size();
        int m = 0;
        std::vector<plane_t> planes;
        for (unsigned i=0; i<n; ++i) {
            plane_t *q = dynamic_cast<plane_t*>(space.get(i));
            assert(q!=0);

            if (m==0) m = q->a.size();
            else assert(m == q->a.size());

            planes.push_back(q->normal_form());
        }
        return planes;
    }
    
    void split_planes(const std::vector<plane_t> &p, 
                      int k,
                      std::vector<plane_t> &neg, 
                      std::vector<plane_t> &pos,
                      std::vector<plane_t> &nix)
    {
        for (unsigned j=0; j<p.size(); j++) {
            if (p[j].a[k] < 0) 
                neg.push_back(p[j]);
            else if (p[j].a[k] > 0) 
                pos.push_back(p[j]);
            else if (p[j].a[k] == 0) 
                nix.push_back(p[j]);
        }        
    }

    // Here, I am assuming that "space" only consists of planes.
    // TODO: 
    bool is_feasible(conjunct_space_t &space) 
    {
        int n = space.size();   // number of inequalities

        std::vector<plane_t> planes = get_normalized_planes(space);
        assert(planes.size() == n);
        int m = planes[0].size();

        //std::cout << "Size is " << n << " x " << m << std::endl;

        for (unsigned k=0; k<m; ++k) {
            // eliminate the k-th variable;
            std::vector<plane_t> neg, pos, nix;
            split_planes(planes, k, neg, pos, nix);
            // std::cout << "neg: " << neg.size() << std::endl;
            // for (auto &pp : neg) std::cout << pp << std::endl;
            // std::cout << "pos: " << pos.size() << std::endl;
            // for (auto &pp : pos) std::cout << pp << std::endl;
            // std::cout << "nix: " << nix.size() << std::endl;
            // for (auto &pp : nix) std::cout << pp << std::endl;

            planes.clear();
            for (auto &x : nix) 
                planes.push_back(x);
            
            // std::cout << "Current content of planes: " << std::endl;
            // for (auto &pp : planes) std::cout << pp << std::endl;

            for (auto &x : neg) {
                for (auto &y : pos) {
                    double xc = x.a[k];
                    double yc = y.a[k];
                    coeff_row_t newrow(m);
                    for (unsigned h=0; h<m; ++h) 
                        //if (h == k) newrow[h] = 0;
                        //else 
                        newrow[h] = x.a[h] - y.a[h] * xc/yc;
                    int s = plane_t::lt; 
                    if (x.sign == plane_t::lte && x.sign == plane_t::lte) 
                        s = plane_t::lte;
                    double b = x.b - y.b *xc/yc;
                    plane_t newplane(newrow, s, b);
                    planes.push_back(newplane);
                }
            }
            // std::cout << "Current content of planes: " << std::endl;
            // for (auto &pp : planes) std::cout << pp << std::endl;                
        }
        // all variables have been eliminated, now there remain only
        // tautologies (0 <= b) or contradictions
 
        // look for contradictions
        for (auto &x : planes) {
            if (x.b < 0) return false;
        }
        return true;
    }
}

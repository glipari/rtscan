#include <cassert>
#include <iostream>
#include <iomanip>
#include "constraint_space.hpp"

namespace Scan {

    const int Plane::lt = -2;
    const int Plane::lte = -1;
    const int Plane::gt = 2;
    const int Plane::gte = 1;
    const int Plane::eq = 0;
    
    AbstractConstraint::AbstractConstraint(size_t n) : num_vars(n) {}

    bool AbstractConstraint::contains(const point_t &p) const
    {
        return is_in(p);
    }

    AbstractConstraint *AbstractConstraint::complement() const
    {
        return negate();
    }


    Plane::Plane(size_t n) : AbstractConstraint(n), a(n), sign(0), b(0)
    {
        for (auto &x : a) x = 0; 
    }
    
    Plane::Plane(const coeff_row_t &coeff, int s, double c) :
        AbstractConstraint(coeff.size()), a(coeff), sign(s), b(c)
    {
    }

    Plane * Plane::copy() const
    {
        return new Plane(*this);
    }
    
    void Plane::print(std::ostream &os) const
    {
        int i = 1;
        for (auto x : a) {
            if (x > 0) 
                os << " +" << std::setw(4) << x << " x(" << i << ")";
            else if (x < 0) 
                os << " " << std::setw(5) << x << " x(" << i << ")";
            else os << "           "; 
            i++;
        } 
        if (sign == Plane::lt)      os << " <  ";
        else if (sign == Plane::lte) os << " <= ";
        else if (sign == Plane::eq)  os << " =  ";
        else if (sign == Plane::gte)  os << " >= ";
        else if (sign == Plane::gt)  os << " >  ";
    
        os << std::setw(5) << b;
    }
    
    bool Plane::is_in(const point_t &p) const
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

    void Plane::change_sign()
    {
        for (auto &x : a) x = -x;
        sign = -sign;
        b = -b;
    }

    Plane Plane::normal_form() const
    {
        Plane x = *this;
        if (x.sign == gt || x.sign == gte) x.change_sign();
        return x;
    }
        

    Plane *Plane::negate() const
    {
        Plane *p = copy();
        p->sign = -sign;
        return p;
    }


    bool Conjunction::is_in(const point_t &p) const
    {
        for (auto &pl : planes) 
            if (!pl.contains(p)) return false;
        return true;
    }

    Conjunction *Conjunction::copy() const
    {
        return new Conjunction(*this);
    }


    AbstractConstraintSet *Conjunction::negate() const
    {
        DisjunctionSet *s = new DisjunctionSet(num_vars);
        for (auto &pp : planes) 
            s->add_constraint(pp.negate());
        return s;
    }
    
    
    void Conjunction::add_plane(const Plane &p)
    {
        assert(p.get_nvars() == get_nvars());
        planes.push_back(p);        
    }

    Plane Conjunction::get(unsigned i) const
    {
        assert(i < planes.size());
        return planes[i];
    }
    
    Plane & Conjunction::at(unsigned i) 
    {
        assert(i < planes.size());
        return planes[i];        
    }


    void Conjunction::print(std::ostream &os) const
    {
        for (unsigned i=0; i<size(); ++i) 
            os << planes[i] << std::endl;
    }


    ConstraintSet::ConstraintSet(size_t n) : AbstractConstraintSet(n), cs()
    {
    }

    ConstraintSet::~ConstraintSet() 
    {
        for (auto x : cs) delete x;
    }

    ConstraintSet::ConstraintSet(const ConstraintSet &s) : AbstractConstraintSet(s)
    {
        for (auto x : s.cs) 
            cs.push_back(x->copy());
    }
    
    void ConstraintSet::add_constraint(const AbstractConstraint &c)
    {
        assert(c.get_nvars() == num_vars);
        AbstractConstraint *p = c.copy();
        cs.push_back(p);
    }

    void ConstraintSet::add_constraint(AbstractConstraint *c)
    {
        assert(c->get_nvars() == num_vars);
        cs.push_back(c);
    }

    size_t ConstraintSet::size() const
    {
        return cs.size();
    }

    ConjunctionSet::ConjunctionSet(size_t n) : ConstraintSet(n)
    {
    }


    AbstractConstraint *ConstraintSet::get(unsigned r) 
    {
        assert(r < cs.size());
        
        return cs.at(r);
    }

    ConjunctionSet * ConjunctionSet::copy() const
    {
        ConjunctionSet *p = new ConjunctionSet(*this);
        return p;
    }
    
    
    bool ConjunctionSet::is_in(const point_t &p) const
    {
        for (auto x : cs) 
            if (!x->contains(p)) return false;
        
        return true;
    }
    
    ConstraintSet * ConjunctionSet::negate() const
    {
        DisjunctionSet *space = new DisjunctionSet(num_vars);
        for (auto c : cs) 
            space->add_constraint(c->negate());
        return space;
    }


    DisjunctionSet::DisjunctionSet(size_t n) : ConstraintSet(n)
    {
    }

    DisjunctionSet * DisjunctionSet::copy() const
    {
        DisjunctionSet *p = new DisjunctionSet(*this);
        return p;
    }
    

    bool DisjunctionSet::is_in(const point_t &p) const
    {
        for (auto x : cs) 
            if (x->contains(p)) return true;
        
        return false;
    }

    ConstraintSet * DisjunctionSet::negate() const
    {
        ConjunctionSet *space = new ConjunctionSet(num_vars);
        for (auto c : cs) 
            space->add_constraint(c->negate());
        return space;
    }

    Conjunction non_negative_space(int n)
    {
        Conjunction res(n);
        for (int i=0; i<n; ++i) {
            coeff_row_t r;
            for (int j=0; j<n; ++j) r.push_back(0);
            r[i] = 1;
            Plane c = {r, Plane::gte, 0};
            res.add_plane(c);
        }
        return res;
    }

    std::vector<Plane> get_normalized_planes(Conjunction &space)
    {
        int n = space.size();
        int m = space.get_nvars();

        std::vector<Plane> planes;
        for (unsigned i=0; i<n; ++i) 
            planes.push_back(space.at(i).normal_form());
        
        return planes;
    }
    
    void split_planes(const std::vector<Plane> &p, 
                      int k,
                      std::vector<Plane> &neg, 
                      std::vector<Plane> &pos,
                      std::vector<Plane> &nix)
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
    bool is_feasible(Conjunction &space) 
    {
        int n = space.size();   // number of inequalities

        std::vector<Plane> planes = get_normalized_planes(space);
        assert(planes.size() == n);
        int m = planes[0].size();

        //std::cout << "Size is " << n << " x " << m << std::endl;

        for (unsigned k=0; k<m; ++k) {
            // eliminate the k-th variable;
            std::vector<Plane> neg, pos, nix;
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
                    int s = Plane::lt; 
                    if (x.sign == Plane::lte && x.sign == Plane::lte) 
                        s = Plane::lte;
                    double b = x.b - y.b *xc/yc;
                    Plane newplane(newrow, s, b);
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

    void ConjunctionSet::print(std::ostream &os) const
    {
        os << "AND { " << std::endl;
        for (unsigned i = 0; i<size(); ++i) {            
            os << *(cs[i]) << std::endl << std::endl;
        }
        os << "}";
    }

    void DisjunctionSet::print(std::ostream &os) const
    {
        os << "OR { " << std::endl;
        for (unsigned i = 0; i<size(); ++i) {            
            os << *(cs[i]) << std::endl << std::endl;
        }
        os << "}";
    }    

    std::ostream& operator<<(std::ostream &os, const AbstractConstraint &s)
    {
        s.print(os);
        return os;
    }
}

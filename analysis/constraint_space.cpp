#include <cassert>
#include "constraint_space.hpp"

namespace Scan {

    bool constraint_t::contains(const point_t &p) const
    {
        return is_in(p);
    }

    constraint_t *constraint_t::complement() const
    {
        return negate();
    }


    plane_t::plane_t(size_t n) : a(n), sign(0), b(0)
    {
        for (auto &x : a) x = 0; 
    }
    
    plane_t::plane_t(const coeff_row_t &coeff, int s, double c) :
        a(coeff), sign(s), b(c)
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
            if (x > 0) os << "+";
            os << x << " X_" << i++;
        } 
        if (s.sign == -2)      os << " <  ";
        else if (s.sign == -1) os << " <= ";
        else if (s.sign == 0)  os << " =  ";
        else if (s.sign == 1)  os << " >= ";
        else if (s.sign == 2)  os << " >  ";
    
        os << s.b;
        return os;
    }
    
    bool plane_t::is_in(const point_t &p) const
    {
        assert(p.size() == a.size());
        double sum = 0;
        for (unsigned i=0; i<a.size(); ++i)
            sum += a[i] * p[i];
        switch(sign) {
        case -2: return sum < b;
            break;
        case -1: return sum <= b;
            break;
        case 0: return sum == b;
            break;
        case 1: return sum > b;
            break;
        case 2: return sum >= b;
            break;
        }
        assert(false);
        return false;
    }

    plane_t *plane_t::negate() const
    {
        plane_t *p = copy();
        p->sign = -sign;
        return p;
    }

    space_t::space_t() : cs()
    {
    }

    space_t::~space_t() 
    {
        for (auto x : cs) delete x;
    }

    space_t::space_t(const space_t &s) 
    {
        for (auto x : s.cs) 
            cs.push_back(x->copy());
    }
    
    void space_t::add_constraint(const constraint_t &c)
    {
        constraint_t *p = c.copy();
        cs.push_back(p);
    }

    void space_t::add_constraint(constraint_t *c)
    {
        cs.push_back(c);
    }

    size_t space_t::size() const
    {
        return cs.size();
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
        disjunct_space_t *space = new disjunct_space_t();
        for (auto c : cs) 
            space->add_constraint(c->negate());
        return space;
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
        conjunct_space_t *space = new conjunct_space_t();
        for (auto c : cs) 
            space->add_constraint(c->negate());
        return space;
    }

    conjunct_space_t non_negative_space(int n)
    {
        conjunct_space_t res;
        for (int i=0; i<n; ++i) {
            coeff_row_t r;
            for (int j=0; j<n; ++j) r.push_back(0);
            r[i] = 1;
            plane_t c = {r, 1, 0};
            res.add_constraint(c);
        }
        return res;
    }

}

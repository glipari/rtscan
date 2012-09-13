#ifndef __CONSTRAINT_SPACE_HPP__
#define __CONSTRAINT_SPACE_HPP__

#include <vector>
#include <cmath>
#include <ostream>

namespace Scan {

    typedef std::vector<double> point_t;
    typedef std::vector<double> coeff_row_t;
    
    /** 
        This class represents a generic constraint.  It is possible to
        check that a point respects the constraint (method contains), and
        to build a complement of this constraint. 
    */ 
    class AbstractConstraint {
    protected:
        virtual bool is_in(const point_t &p) const = 0;
        size_t num_vars;
    public:
        AbstractConstraint(size_t nvars);

        virtual AbstractConstraint *copy() const = 0;
        virtual AbstractConstraint *negate() const = 0;

        bool contains(const point_t &p) const;
        AbstractConstraint *complement() const;
        size_t get_nvars() const { return num_vars; }

    };    

    /** 
        This class models a linear inequality, and it is one type of
        constraint, which is made by one single inequality.

        sign represents the sign of the inequality. The constants have
        been choosen to so to make sure that it is possible to
        complement the constraint by changing sign.
    */ 
    class Plane : public AbstractConstraint {
    public:
        Plane(size_t n);
        Plane(const coeff_row_t &coeff, int s, double c);

        coeff_row_t a;
        int sign;  // lt: -2; lte : -1; eq : 0; gt : 2; gte : 1
        double b;

        static const int lt;
        static const int lte;
        static const int gt;
        static const int gte;
        static const int eq;

        size_t size() const { return a.size(); }
        
        void change_sign();
        Plane normal_form() const;

        Plane *negate() const;
        Plane *copy() const;

    protected:
        bool is_in(const point_t &p) const;
    };   
    
    /** Pretty print of the linear inequality */
    std::ostream& operator<<(std::ostream &os, const Plane &s);

    class AbstractConstraintSet : public AbstractConstraint {
    public:
        AbstractConstraintSet(size_t n) : AbstractConstraint(n) {}
        // here I should add the intersection functions
    };

    class Conjunction : public AbstractConstraintSet {
        std::vector<Plane> planes;
        bool is_in(const point_t &p) const;
    public:
        Conjunction(size_t n) : AbstractConstraintSet(n) {}
        void add_plane(const Plane &p);
        Plane get(unsigned i) const;
        Plane& at(unsigned i);
        size_t size() const { return planes.size(); }

        Conjunction *copy() const;
        AbstractConstraintSet *negate() const;
    };

    std::ostream& operator<<(std::ostream &os, const Conjunction &s);
    /** 
        This class models a set of constraints.  The copy constructor
        works as expected. Notice that it is not possible to
        instantiate objects of this type, as the virtual functions
        is_in() and negate() are virtual. 
        
        @todo see if it is necessary to provide an assignment operator
        as well.
    */
    class ConstraintSet : public AbstractConstraintSet {
    protected:
        std::vector<AbstractConstraint *> cs;

        ConstraintSet *copy() const = 0;
    public:
        ConstraintSet(size_t n);
        ConstraintSet(const ConstraintSet &s);
        ~ConstraintSet(); 

        void add_constraint(const AbstractConstraint &c);
        void add_constraint(AbstractConstraint *c);
        AbstractConstraint *get(unsigned r);
        size_t size() const;
    };

  
    /**
       This space is the conjunction (AND) of the constraints. That is, 
       a point belongs to this region if it verifies all the included 
       constraints.
    */
    class ConjunctionSet : public ConstraintSet {
    protected:
        ConjunctionSet *copy() const;
        bool is_in(const point_t &p) const;
        ConstraintSet *negate() const;        
    public:
        ConjunctionSet(size_t n);
    };

     /**
       This space is the disjunction (OR) of the constraints. That is,
       a point belongs to this space if it verifies at least one of the 
       included constraints.
     */
    class DisjunctionSet : public ConstraintSet {
    protected:
        DisjunctionSet *copy() const;
        bool is_in(const point_t &p) const;
        ConstraintSet *negate() const;

    public:
        DisjunctionSet(size_t n);

    };
                
    /// I x >= 0
    Conjunction non_negative_space(int n);
    
    /**
       Uses Fourier Motzkin to see if the system of inequalities is
       feasible. It requires that space is a system of inequalities
       (planes), otherwise will trigger a false assertion.
    */
    bool is_feasible(Conjunction &space);
}

#endif

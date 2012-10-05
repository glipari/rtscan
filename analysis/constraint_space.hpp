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
        size_t num_vars;
    public:
        AbstractConstraint(size_t nvars);

        /**
           Creates an object which is a copy of the current object
        */
        virtual AbstractConstraint *copy() const = 0;
        /**
           Creates an object which is the complement of the current
           object
         */
        virtual AbstractConstraint *negate() const = 0;

        /**
           Assigns a value val at variable at position varindex
         */
        virtual void project(unsigned varindex, double val) = 0;

        /**
           Returns true is the point p is inside the constraints
         */
        virtual bool is_in(const point_t &p) const = 0;

        /**
           Returns the number of variables in the constraints
         */
        size_t get_nvars() const { return num_vars; }
        
        /**
           prints this constraint
         */
        virtual void print(std::ostream &os) const = 0;
    };    

    /** 
        This class models a linear inequality, and it is one type of
        constraint, which is made by one single inequality.

        sign represents the sign of the inequality, which can be one
        of the constants lte, lt, gt, gte. Notice that it is not
        possible to specify equality (this must be done with two
        different inequalities.
    */ 
    class Plane : public AbstractConstraint {
    public:
        Plane(size_t n);
        Plane(const coeff_row_t &coeff, int s, double c);

        coeff_row_t a;
        int sign;  
        double b;

        static const int lt;
        static const int lte;
        static const int gt;
        static const int gte;

        size_t size() const { return a.size(); }
        
        /** 
            Changes sign to all coefficient, to the constant b and to
            the inequality sign. The resulting constraint is perfectly
            equivalent to the previous one */
        void change_sign();
        
        /**
           Returns a new plane in normal form (which has lt or lte as
           sign). If sign == gt or sign == gte, it calls change sign.
        */
        Plane& normal_form();
        
        Plane *negate() const;
        Plane *copy() const;
        void project(unsigned varindex, double val);
        void print(std::ostream &os) const;
        bool is_in(const point_t &p) const;
    };   
    
    class AbstractConstraintSet : public AbstractConstraint {
    public:
        AbstractConstraintSet(size_t n) : AbstractConstraint(n) {}
        // here I should add the intersection functions
    };


    class PlaneSet : public AbstractConstraintSet {
    protected:
        std::vector<Plane> planes;
    public:
        PlaneSet(size_t n) : AbstractConstraintSet(n) {}

        /**
           Add a plane to the set of constraints
         */
        void add_plane(const Plane &p);
        /**
           Returns a copy of the plane at position i
         */
        Plane get(unsigned i) const;
        /**
           Returns a reference to the plane at position i
         */
        Plane& at(unsigned i);
        /**
           Returns the number of inequalities inside the set
         */
        size_t size() const { return planes.size(); }

        /**
           All planes are modified in normal form.
         */
        void normalize();

        void project(unsigned varindex, double val);
    };

    /** 
        This class models a set of linear inequalities in conjunctive
        form. The resulting subspace is convex.  It is possible to
        check properties more efficiently for this class.
    */
    class Conjunction : public PlaneSet {
    public:
        Conjunction(size_t n) : PlaneSet(n) {}

        Conjunction *copy() const;
        AbstractConstraintSet *negate() const;
        void print(std::ostream &os) const;
        bool is_in(const point_t &p) const;

        /**
           Reduces the number of inequalities by eliminating redundant ones.
         */
        void tighen();
    };

    /** 
        This class models a set of linear inequalities in conjunctive
        form. The resulting subspace is convex.  It is possible to
        check properties more efficiently for this class.
    */
    class Disjunction : public PlaneSet {
    public:
        Disjunction(size_t n) : PlaneSet(n) {}

        Disjunction *copy() const;
        AbstractConstraintSet *negate() const;
        void print(std::ostream &os) const;
        void tighen(); // eliminates redundant planes
        bool is_in(const point_t &p) const;
    };

    /** 
        This class models a set of constraints.  The copy constructor
        works as expected. Notice that it is not possible to
        instantiate objects of this type, as the virtual functions
        is_in() and negate() are virtual.         
    */
    class ConstraintSet : public AbstractConstraintSet {
    protected:
        std::vector<AbstractConstraint *> cs;

        ConstraintSet *copy() const = 0;
    public:
        ConstraintSet(size_t n);
        ConstraintSet(const ConstraintSet &s);
        ~ConstraintSet(); 

        ConstraintSet &operator=(const ConstraintSet &other);

        void add_constraint(const AbstractConstraint &c);
        void add_constraint(AbstractConstraint *c);
        AbstractConstraint *get(unsigned r);
        size_t size() const;

        void project(unsigned varindex, double val);
    };

  
    /**
       This space is the conjunction (AND) of the constraints. That
       is, a point belongs to this region if it verifies all the
       included constraints.
    */
    class ConjunctionSet : public ConstraintSet {
    protected:
        ConjunctionSet *copy() const;
        ConstraintSet *negate() const;        
    public:
        ConjunctionSet(size_t n);
        void print(std::ostream &os) const;
        bool is_in(const point_t &p) const;
    };

     /**
       This space is the disjunction (OR) of the constraints. That is,
       a point belongs to this space if it verifies at least one of
       the included constraints.
     */
    class DisjunctionSet : public ConstraintSet {
    protected:
        DisjunctionSet *copy() const;
        ConstraintSet *negate() const;

    public:
        DisjunctionSet(size_t n);
        void print(std::ostream &os) const;
        bool is_in(const point_t &p) const;
    };

    /** Pretty print of the sets */
    std::ostream& operator<<(std::ostream &os, const AbstractConstraint &s);
                
    /**
       Generates the constraint set I x >= 0
    */
    Conjunction non_negative_space(int n);
    
    /**
       Uses Fourier Motzkin to see if the system of inequalities is
       feasible.
    */
    bool fme_is_feasible(const Conjunction &space);
}

#endif

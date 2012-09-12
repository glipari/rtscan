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
    class constraint_t {
    protected:
        virtual bool is_in(const point_t &p) const = 0;

        size_t num_vars;
    public:
        constraint_t(size_t nvars);

        virtual constraint_t *copy() const = 0;
        virtual constraint_t *negate() const = 0;

        bool contains(const point_t &p) const;
        constraint_t *complement() const;
        size_t get_nvars() const { return num_vars; }

    };    

    /** 
        This class models a linear inequality, and it is one type of
        constraint, which is made by one single inequality.

        sign represents the sign of the inequality. The constants have
        been choosen to so to make sure that it is possible to
        complement the constraint by changing sign.
    */ 
    class plane_t : public constraint_t {
    public:
        plane_t(size_t n);
        plane_t(const coeff_row_t &coeff, int s, double c);

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
        plane_t normal_form() const;
    protected:
        bool is_in(const point_t &p) const;
        plane_t *negate() const;
        plane_t *copy() const;
    };   
    
    /** Pretty print of the linear inequality */
    std::ostream& operator<<(std::ostream &os, const plane_t &s);

    /** 
        This class models a set of constraints.  The copy constructor
        works as expected. Notice that it is not possible to
        instantiate objects of this type, as the virtual functions
        is_in() and negate() are virtual. 

        @todo see if it is necessary to provide an assignment operator
        as well.
    */
    class space_t : public constraint_t {
    protected:
        std::vector<constraint_t *> cs;

        space_t *copy() const = 0;
    public:
        space_t(size_t n);
        space_t(const space_t &s);
        ~space_t(); 

        void add_constraint(const constraint_t &c);
        void add_constraint(constraint_t *c);
        constraint_t *get(unsigned r);
        size_t size() const;
    };

  
    /**
       This space is the conjunction (AND) of the constraints. That is, 
       a point belongs to this region if it verifies all the included 
       constraints.
    */
    class conjunct_space_t : public space_t {
    protected:
        conjunct_space_t *copy() const;
        bool is_in(const point_t &p) const;
        space_t *negate() const;        
    public:
        conjunct_space_t(size_t n);
    };

     /**
       This space is the disjunction (OR) of the constraints. That is,
       a point belongs to this space if it verifies at least one of the 
       included constraints.
     */
    class disjunct_space_t : public space_t {
    protected:
        disjunct_space_t *copy() const;
        bool is_in(const point_t &p) const;
        space_t *negate() const;

    public:
        disjunct_space_t(size_t n);

    };
                
    /// I x >= 0
    conjunct_space_t non_negative_space(int n);
    
    /**
       Uses Fourier Motzkin to see if the system of inequalities is
       feasible. It requires that space is a system of inequalities
       (planes), otherwise will trigger a false assertion.
    */
    bool is_feasible(conjunct_space_t &space);
}

#endif

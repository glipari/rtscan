#ifndef __SEQUENCES_HPP__
#define __SEQUENCES_HPP__

#include <algorithm>
#include <functional>
#include <string>
#include <ostream>

namespace Scan {
    /**
       Selects all elements in the intersection between sequence [a1,
       b1) and [a2, b2) and inserts them in the sequence starting at
       a3.

       Equality is established by function cmp that must take two
       elements, one from the first sequence, and the second one from
       the second sequence.

       The elements inserted in the final sequence are all taken from
       [a1, b1).
     */
    template <class Iter1, class Iter2, class Iter3, class Fun>
    void intersect(Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Iter3 a3, Fun cmp)
    {
        using namespace std::placeholders;
        for (Iter1 i = a1; i != b1; ++i) {
            auto f = std::bind(cmp, *i, _1);
            if (find_if(a2, b2, f) != b2) { // found it, so insert
                *a3 = *i;
                ++a3;
            }
        }
    }

    /**
       Given two sequences [a1, b1) and [a2, b2), selects all elements
       in the first sequence but not in the second sequence, and
       inserts then in the third sequence starting from a3.

       Equality is established by function cmp that must take two
       elements, one from the first sequence, and the second one from
       the second sequence.
     */
    template <class Iter1, class Iter2, class Iter3, class Fun>
    void subtract(Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Iter3 a3, Fun cmp)
    {
        using namespace std::placeholders;
        for (Iter1 i = a1; i != b1; ++i) {
            auto f = std::bind(cmp, *i, _1);
            if (find_if(a2, b2, f) == b2) {// not found it, so insert
                *a3 = *i;
                ++a3;
            }
        }
    }

    /**
       Selects all the elements from sequence [a1, b1) that verify
       predicate pred, and inserts them in the sequence starting from
       position a2.
    */
    template <class Iter1, class Iter2, class Fun>
    void select(Iter1 a1, Iter1 b1, Iter2 a2, Fun pred)
    {
        for (Iter1 i = a1; i != b1; ++i, ++a2) {
            if (pred(*i)) *a2 = *i;
        }
    }


    /** 
        Computes the sum of the largest nproc elements in the
        sequence 
    */
    template <class Iter>
    double sum_max_n(Iter v_begin, Iter v_end, unsigned nproc)
    {
        double s = 0.0;
        sort(v_begin, v_end);
        reverse(v_begin, v_end);
        unsigned k = 0;
        for (Iter i = v_begin; i != v_end; ++i) {
            if (k >= nproc) break; 
            s += *i; 
            k++;
        }
        return s;
    }

    template <typename OutIt>
    OutIt string_split(const std::string &text, const std::string seps, 
                       OutIt out, bool empty_tokens = false)
    {
        size_t start = 0;
        size_t end   = 0;

        if (!empty_tokens) start = text.find_first_not_of(seps, 0);
        
        while((end = text.find_first_of(seps, start)) != std::string::npos)
        {
            *out++ = text.substr(start, end - start);

            if (empty_tokens) start = end + 1;
            else start = text.find_first_not_of(seps, end);
            
        }
        if (start != std::string::npos) *out++ = text.substr(start);
        
        return out;
    }
}

#endif

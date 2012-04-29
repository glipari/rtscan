#ifndef __SEQUENCES_HPP__
#define __SEQUENCES_HPP__

#include <algorithm>
#include <functional>
#include <string>

namespace Scan {
    template <class Container, class Iter1, class Iter2, class Fun>
    void intersect(Container &c, Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Fun cmp)
    {
        using namespace std::placeholders;
        for (Iter1 i = a1; i != b1; ++i) {
            auto f = std::bind(cmp, *i, _1);
            if (find_if(a2, b2, f) != b2) // found it, so insert
                c.insert(c.end(), *i);
        }
    }

    template <class Container, class Iter1, class Iter2, class Fun>
    void subtract(Container &c, Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Fun cmp)
    {
        using namespace std::placeholders;
        for (Iter1 i = a1; i != b1; ++i) {
            auto f = std::bind(cmp, *i, _1);
            if (find_if(a2, b2, f) == b2) // not found it, so insert
                c.insert(c.end(), *i);
        }
    }

    template <class Container, class Iter1, class Fun>
    void select(Container &c, Iter1 a1, Iter1 b1, Fun pred)
    {
        for (Iter1 i = a1; i != b1; ++i) {
            if (pred(*i)) c.insert(c.end(), *i);
        }
    }


    /** computes the sum of the largest nproc elements in the
     * sequence */
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
    OutIt string_split(const std::string &text, const std::string seps, OutIt out, bool empty_tokens = false)
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

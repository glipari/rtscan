#ifndef __TASK_SET2_HPP__
#define __TASK_SET2_HPP__

#include <algorithm>
#include <functional>

namespace Scan {
    using namespace std::placeholders;

    template <class Container, class Iter1, class Iter2, class Fun>
    void intersect(Container &c, Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Fun cmp)
    {
        for (Iter1 i = a1; i != b1; ++i) {
            auto f = std::bind(cmp, *i, _1);
            if (find_if(a2, b2, f) != b2) // found it, so insert
                c.insert(c.end(), *i);
        }
    }

    template <class Container, class Iter1, class Iter2, class Fun>
    void subtract(Container &c, Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Fun cmp)
    {
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
}

#endif

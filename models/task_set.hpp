#ifndef __TASK_SET2_HPP__
#define __TASK_SET2_HPP__

#include <algorithm>

namespace Scan {
    template <class Container, class Iter1, class Iter2>
    void intersect(Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Container &c)
    {
        for (Iter1 i = a1; i != b1; ++i) {
            if (find(a2, b2, *i) != b2) // found it, so insert
                c.insert(c.end(), *i);
        }
    }

    template <class Container, class Iter1, class Iter2>
    void subtract(Iter1 a1, Iter1 b1, Iter2 a2, Iter2 b2, Container &c)
    {
        for (Iter1 i = a1; i != b1; ++i) {
            if (find(a2, b2, *i) == b2) // not found it, so insert
                c.insert(c.end(), *i);
        }
    }

}

#endif

#pragma once

#include <algorithm>

namespace Scan {
    template <class T>
    struct IntervalType {
        T first;
        T second;
        IntervalType(const T &a , const T &b) : first(a), second(b) {}
        bool is_empty() const { return first > second; }
    };

    template <class T>
    IntervalType<T> interval_intersect(const IntervalType<T> &a, const IntervalType<T> &b) {
        return IntervalType<T> (std::max(a.first, b.first), 
                                std::min(a.second, b.second));
    }

    template <typename Iterator, typename T, typename OutIt> 
    void interval_intersect_all(Iterator begin, Iterator end, IntervalType<T> i, OutIt out)
    {
        for (Iterator k = begin; k != end; ++k) {
            IntervalType<T> x = interval_intersect(*k, i);
            if (!x.is_empty()) *out++ = x;
        } 
    }

    template<class T>
    std::ostream &operator<<(std::ostream &os, const Scan::IntervalType<T> &i) {
        os << "[" << i.first << ", " << i.second << "]";
        return os;
    }

    template<class Iterator>
    void print_sequence(Iterator begin, Iterator end, std::ostream &os)
    {
        os << "{";
        for (Iterator k = begin; k!= end; ++k) {
            os << *k; 
            if (k != end-1) os << ", ";
        }
        os << "}";
    }
}


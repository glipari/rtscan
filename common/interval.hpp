#pragma once

#include <algorithm>

namespace Scan {
    /** 
       An interval of values. The interval includes the extremes, so
       it is always closed.
     */
    template <class T>
    struct IntervalType {
        T first;
        T second;
        IntervalType(const T &a , const T &b) : first(a), second(b) {}

        /// True if the interval is empty (does not contain ANY point)
        bool is_empty() const { return first > second; }
    };

    /**
      Returns the intersection of two intervals. 
     */
    template <class T>
    IntervalType<T> interval_intersect(const IntervalType<T> &a, const IntervalType<T> &b) {
        return IntervalType<T> (std::max(a.first, b.first), 
                                std::min(a.second, b.second));
    }


    /**
      Given a set of intervals, it returns the set of intervals
      intersection obtained as intersection of each one of the input
      intervals with interval i.


      @param begin the iterator to the first element in the sequence
      @param end the iterator to the after-last element in the sequence
      @param i input interval
      @param out iterator to the output sequence
     */
    template <typename Iterator, typename T, typename OutIt> 
    void interval_intersect_all(Iterator begin, Iterator end, IntervalType<T> i, OutIt out)
    {
        for (Iterator k = begin; k != end; ++k) {
            IntervalType<T> x = interval_intersect(*k, i);
            if (!x.is_empty()) *out++ = x;
        } 
    }

    /**
       Prints an interval on an output stream
     */
    template<class T>
    std::ostream &operator<<(std::ostream &os, const Scan::IntervalType<T> &i) {
        os << "[" << i.first << ", " << i.second << "]";
        return os;
    }

    /**
       Prints a sequence (not only intervals) on an output stream.

       @todo move this somewhere else
     */
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


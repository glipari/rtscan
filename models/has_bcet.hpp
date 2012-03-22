#ifndef __HAS_BCET_HPP__
#define __HAS_BCET_HPP__

#include <common/exceptions.hpp>

namespace Scan {
    class HasBCET {
        double bcet;
    public:
        HasBCET() : bcet(0) {}
        double get_bcet() { return bcet; }
        void set_bcet(double b) { 
            if (bcet < 0) THROW_EXC(NegativeValue);
            bcet = b; 
        }
    };
}

#endif

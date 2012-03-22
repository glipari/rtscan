#include <iostream>

#include <common/model.hpp>

namespace Scan {
    
    bool ModelTemplate::check(PropertySet &ps) 
    {
        for (auto s : required) {
            try {
                ps.contains(s);
            } catch (PropertyNotFound &err) {
                // TODO remember to define a global error logging  
                std::cerr << "Required Property " << s << " not found" << std::endl;
                return false;
            }
        }
        // TODO check and set optional values
    
        return true;
    }
}

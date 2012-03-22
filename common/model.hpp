#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector> 

#include <common/property.hpp>
            
/**
   Rationale for the design.

   The user should be able to define its own task model when designing
   an algorithm.  There are algorithms that need a "jitter", others
   need an offset, etc.

   Therefore we need: 

   - a generic "task" structure, able to contain almost every
     parameter, defined once and for all. This data structure must be
     "dynamic", in the sense that it should contain a variable number
     of "variables", each variable has a type, a name and a value.

   - Each algorithm defines its own data structures, and a way to
     convert from and to the general data structure.
   
   - Moreover, the algorithm needs to check the presence of certain
     variables (e.g. the jitter), and having constraints on other
     variables (for example, deadlines equal to periods).

   - Problem no 1: reading / writing the general data structure on a
     file: this should be easy, a simple parser will do. 

   - Problem no 2: convert to and from specific data structures. Of
     course the "algorithm designer" can do it as he whishes, but to
     reduce the work, we should be able to provide him with
     appropriate tools for simplyfing the work.

   - Problem no 3: check consistency of the model. This is the most
     difficult thing, because to do that "automatically" we need a big
     infrastructure. Let's divide the problem into 3 steps:

     a) check that certain mandatory elements are in the structure.
     this ought to be easy, since it suffices to look-up for variable
     names (and types, maybe).
     
     b) check and assign default values to optional elements.     
*/
namespace Scan {
    struct ModelTemplate {
        std::vector<std::string> required;
        std::vector<std::pair<std::string, std::string> > optional;
        
        ModelTemplate(const std::vector<std::string> &r, 
                      const std::vector<std::pair<std::string,std::string> > &o) :
            required(r), optional(o) {}
         
        /**
           Checks that a certain property set contains the required
           fields. Also, it looks for optional fields, and if not
           present, it assigns them a default value.
        */
        bool check(PropertySet &s);
    };
}
#endif

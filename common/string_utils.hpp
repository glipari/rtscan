#include <string>
#include <vector>

namespace StrUtils {
    /** removes spaces from the beginning and from the end of the string */
    std::string remove_spaces(const std::string &tk);
    
    /** split the string into a vector of words, using sep as separator */
    std::vector<std::string> split(const std::string &code, const std::string &sep);    
}

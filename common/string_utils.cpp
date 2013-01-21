#include <common/string_utils.hpp>

using namespace std;

namespace StrUtils {
/** removes spaces from the beginning and from the end of the string */
std::string remove_spaces(const std::string &tk)
{
    std::string temp = tk;
    std::string::size_type pos = 0;

    if (tk == "") return temp;
    // remove spaces at the beginning...
    temp.erase(0, temp.find_first_not_of(" \n\t"));
    pos = temp.find_last_not_of(" \n\t");
    temp.erase(pos+1, temp.size() - pos - 1);
    return temp;
}

/** split the string into a vector of words, using sep as separator */
std::vector<string> split(const std::string &code, const std::string &sep)
{
    std::vector<std::string> temp;
    std::string::size_type pos = 0;
    std::string::size_type old_pos = 0;
    int count = 0;
    
    while (pos != std::string::npos) {
        pos = code.find(sep, old_pos);
        if (pos != std::string::npos) { 
            temp.push_back(remove_spaces(code.substr(old_pos,pos-old_pos)));
            old_pos = pos + sep.size();
            count ++;
        }
        else {
            temp.push_back(remove_spaces(code.substr(old_pos, code.size()
                                                     - old_pos)));
        }
    }
    
    return temp;
}

}

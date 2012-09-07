#include <iostream>
#include <string>
#include <vector>

#include <models/taskres_parser.hpp>

using namespace std;
using namespace Scan;

int main()
{
    string input="SYS task a {c=2 d=10 p=10} task b {c=3 d=8 p=15} END";

    vector<TaskRes> tset;
    vector<Resource> rset;
    parse_taskres(input, tset, rset);
    for (unsigned i=0; i<tset.size(); ++i) {
        cout << tset[i] << endl;
    }
    for (unsigned i=0; i<rset.size(); ++i) {
        cout << rset[i] << endl;
    }
                                           
}

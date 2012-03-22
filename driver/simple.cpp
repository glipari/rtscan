#include <iostream>
#include <string>

#include <models/taskset_parser.hpp>

using namespace std;
using namespace Scan;

int main()
{
    string input="task a {c=2, d=10, p=10} task b {c=3, d=8, p=15}";

    TaskSet tset = parse_tasks(input);
    for (unsigned i=0; i<tset.size(); ++i) {
        cout << tset[i] << endl;
    }

    vector<pair< string, int> > v = {{"Paperino", 1}, {"Pluto", 2}};
    for (auto i : v) 
        cout << i.first << ", " << i.second << endl;
                                           
}

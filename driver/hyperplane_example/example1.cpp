#include <iostream>
#include <vector>
#include <analysis/hyperplane.hpp>

using namespace std;
using namespace Scan;

int main()
{
    vector<Task> tasks = {
        {1,10,10},
        {1,10,10},
        {1,20,20},
        {1,20,20}
    };

    ConjunctionSet space = create_space(tasks);
    cout << space << endl;
}

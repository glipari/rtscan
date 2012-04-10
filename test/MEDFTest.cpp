#include "gtest/gtest.h"

#include <analysis/medf_rta.hpp>

using namespace std;
using namespace Scan;

TEST(MEDFRTA, Simple1)
{
    vector<Task> tset = {
        Task(5,20,20),
        Task(4,16,16),
        Task(7,28,28), 
        Task(9,36,36)
    };

    EXPECT_EQ(true, rta_compl_edf(2, tset));
}

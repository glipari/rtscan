#include "gtest/gtest.h"
#include <vector>
#include <sstream>
#include <common/property_parser.hpp>
#include <common/property.hpp>

using namespace std;
using namespace Scan;

TEST(PropertyParser, simple)
{
    stringstream ss;
    ss << "sys(s) { seq (pippo) { c=1 p=5 par (pluto) {c=4 d=10} } }";
    PropertyList plist;
    EXPECT_TRUE(parse_properties(ss, "string", plist));
    
    //PrintPropertyVisitor vis;
    //vis(plist);
}


#include "catch.hpp"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <common/property_parser.hpp>
#include <common/property.hpp>

using namespace std;
using namespace Scan;

TEST_CASE("PropertyParser, simple")
{
    stringstream ss;
    ss << "sys(s) { seq (pippo) { c=1; p=5; par (pluto) {c=4; d=10;}; }; };";
    PropertyList plist;
    CHECK(parse_properties(ss, "string", plist));
}

TEST_CASE("PropertyParser, string_ex")
{
    stringstream ss;
    ss << "sys(s) { c=1; p=\"long and longer\"; nomore=pippo; };";
    PropertyList plist;
    bool r = false;
    try {
        r = parse_properties(ss, "string", plist);
    } catch(const std::runtime_error& e) {
        cout << e.what() << endl;
    }
    CHECK(r);
}


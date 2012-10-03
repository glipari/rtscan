#ifndef __PROPERTY_PARSER_HPP__
#define __PROPERTY_PARSER_HPP__

#include <common/property.hpp>

namespace Scan {

    struct PrintPropertyVisitor : public boost::static_visitor<> {
        int indent_level;
        PrintPropertyVisitor();
        void operator()(const Property &p);
        void operator()(const PropertyList &pl);
    };

    bool parse_properties(std::istream &is, const std::string &filename,
        PropertyList &pset);
}

#endif

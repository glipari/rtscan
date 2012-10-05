#include <string>
#include <iostream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <common/gen_parse.hpp>
#include <common/property_parser.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    Scan::Property, 
    (std::string, name)
    (std::string, value)
    )

BOOST_FUSION_ADAPT_STRUCT(
    Scan::PropertyList,
    (std::string, type)
    (std::string, name)
    (std::vector< Scan::PropertyList::Element >, children)
    )

namespace Scan {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;

    template <typename Iterator>
    struct property_set_grammar : qi::grammar<Iterator, PropertyList(),
                                              ascii::space_type>
    {
        qi::rule<Iterator, Property(), ascii::space_type> prop;
        qi::rule<Iterator, std::string(), ascii::space_type> name;
        qi::rule<Iterator, std::string(), ascii::space_type> type;
        qi::rule<Iterator, std::string(), ascii::space_type> value;
        qi::rule<Iterator, PropertyList(), ascii::space_type> plist;

        property_set_grammar() : 
            property_set_grammar::base_type(plist, "Set of Properties") {
            using qi::lit;
            using qi::alpha;
            using qi::alnum;
            using qi::lexeme;

            name = lexeme[alpha >> *alnum];
            type = lexeme[alpha >> *alnum];
            value = lexeme[*alnum];

            prop = name >> '=' > value;
            plist = type >> '(' > name > ')' > '{' > *(prop | plist) >> '}';
        }
    };   

    bool parse_properties(std::istream &is, const std::string &filename,
        PropertyList &pset)
    {
        return myparse<property_set_grammar, PropertyList>(is, filename, pset);
    }

    PrintPropertyVisitor::PrintPropertyVisitor() : indent_level(0) {}

    void PrintPropertyVisitor::operator()(const Property &p) 
    {
        for (unsigned i=0; i<indent_level; ++i) std::cout << "  ";
        std::cout << p.name << "=" << p.value << std::endl;
    }

    void PrintPropertyVisitor::operator()(const PropertyList &pl) 
    {
        for (int i=0; i<indent_level; ++i) std::cout << "  ";
        std::cout << pl.type << "(" << pl.name << ") {" << std::endl;
        indent_level++;
        for (auto x : pl.children) 
            boost::apply_visitor(std::ref(*this), x);
        indent_level--;
        for (int i=0; i<indent_level; ++i) std::cout << "  ";
        std::cout << "}" << std::endl;
    }
}

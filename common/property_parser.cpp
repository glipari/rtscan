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

    template <typename Iterator, typename Skipper>
    struct property_set_grammar : qi::grammar<Iterator, PropertyList(),
                                              Skipper>
    {
        qi::rule<Iterator, Property(), Skipper> prop;
        qi::rule<Iterator, std::string(), Skipper> name;
        qi::rule<Iterator, std::string(), Skipper> type;
        qi::rule<Iterator, std::string(), Skipper> value;
        qi::rule<Iterator, std::string(), Skipper> value_simple;
        qi::rule<Iterator, std::string(), Skipper> value_quoted;
        qi::rule<Iterator, PropertyList(), Skipper> plist;

        property_set_grammar() : 
            property_set_grammar::base_type(plist, "Set of Properties") {
            using qi::lit;
            using qi::alpha;
            using qi::alnum;
            using qi::lexeme;
            using qi::char_;

            name = lexeme[alpha >> *alnum];
            type = lexeme[alpha >> *alnum];
            value_simple = lexeme[*(alnum - lit('"'))];
            value_quoted = lit('"') > lexeme[*(char_ - lit('"'))] > lit('"');
            value = (value_quoted | value_simple);

            prop = name >> '=' > value > ';';
            plist = type >> '(' > name > ')' > '{' > *(prop | plist) >> '}' > ';';
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

    
    GenPropertyVisitor::GenPropertyVisitor() : name(""), type("") 
    {
    }
        

    template <class T>
    void check_and_store_values(std::map<std::string, T> &m, const std::string &n, T v)
    {
        if (m.find(n) != m.end()) 
            THROW_EXC(ValueAlreadySet, "value already set");
        else m[n] = v;
    }

    /** search an array of SVectors, to see if a certain keyword exists */
    template<class SV>
    std::string check_keyword(const std::vector<SV> &v, const std::string &name)
    {
        for (auto const &x : v) {
            std::string k = x.first[0];
            for (auto const &y : x.first)
                if (name ==  y) return k;
        } 
        return "";
    }

    void GenPropertyVisitor::operator()(const Property &p) 
    {
        std::string kd = check_keyword(keywords_double, p.name);
        std::string ki = check_keyword(keywords_int, p.name);
        std::string ks = check_keyword(keywords_string, p.name);
        if (kd != "") {
            if (d_values.find(kd) != d_values.end()) THROW_EXC(ValueAlreadySet, "value already set");
            else d_values[kd] = p.get_double();
        }
        if (ki != "") {
            if (i_values.find(ki) != i_values.end()) THROW_EXC(ValueAlreadySet, "value already set");
            else i_values[ki] = p.get_int();
        }
        if (ks != "") {
            if (s_values.find(ks) != s_values.end()) THROW_EXC(ValueAlreadySet, "value already set");
            else s_values[ks] = p.get_value();
        }
    }

    void GenPropertyVisitor::operator()(const PropertyList &pl) 
    {
        name = pl.name;
        type = pl.type;
        for (auto &x : pl.children) 
            boost::apply_visitor(std::ref(*this), x);            
    } 


    bool GenPropertyVisitor::check_mandatory() const
    {
        // for every mandatory field, there must be a field in d_values or i_values
        for (auto &x : mandatory_keys) {
            bool f = false;
            for (auto &y : d_values) 
                if (x == y.first) f = true;
            for (auto &y : i_values) 
                if (x == y.first) f = true;
            for (auto &y : s_values) 
                if (x == y.first) f = true;
            if (!f) {
                std::cerr << "Mandatory property " << x << " not set in " 
                          << type << "(" << name << ")" << std::endl;
                return false;
            }
        }
        return true;
    }
	   

    void GenPropertyVisitor::check_defaults()
    {
	for (auto &x : keywords_double) {
		std::string & keyword = x.first[0];
            	if (d_values.find(keyword) == d_values.end())
            		d_values[keyword] = x.second;
	}
	for (auto &x : keywords_int) {
		std::string & keyword = x.first[0];
            	if (i_values.find(keyword) == i_values.end())
            		i_values[keyword] = x.second;
	}
	for (auto &x : keywords_string) {
		std::string & keyword = x.first[0];
            	if (s_values.find(keyword) == s_values.end())
            		s_values[keyword] = x.second;
	}
    }

    void GenPropertyVisitor::add_double_parameter(const SVector &v, double x, bool mandatory) 
    {
        keywords_double.push_back(make_pair(v, x));
        if (mandatory) mandatory_keys.push_back(v[0]);
    }

    void GenPropertyVisitor::add_int_parameter(const SVector &v, int x, bool mandatory) 
    {
        keywords_int.push_back(make_pair(v, x));
        if (mandatory) mandatory_keys.push_back(v[0]);
    }

    void GenPropertyVisitor::add_string_parameter(const SVector &v, std::string x, bool mandatory) 
    {
        keywords_string.push_back(make_pair(v, x));
        if (mandatory) mandatory_keys.push_back(v[0]);
    }
}

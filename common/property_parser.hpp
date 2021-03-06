#ifndef __PROPERTY_PARSER_HPP__
#define __PROPERTY_PARSER_HPP__

#include <map>
#include <vector>
#include <string>
#include <common/property.hpp>

namespace Scan {

    DECL_EXC(ValueAlreadySet);
    DECL_EXC(IncorrectType);

    struct PrintPropertyVisitor : public boost::static_visitor<> {
        int indent_level;
        PrintPropertyVisitor();
        void operator()(const Property &p);
        void operator()(const PropertyList &pl);
    };


    class GenPropertyVisitor : public boost::static_visitor<> {
    protected:

        typedef std::vector< std::string > SVector;
        typedef std::pair< SVector, double > SVDouble; 
        typedef std::pair< SVector, int > SVInt; 
        typedef std::pair< SVector, std::string > SVString; 
        
        std::vector< std::string > mandatory_keys;
        std::vector< std::pair< SVector, double> > keywords_double;
        std::vector< std::pair< SVector, int> > keywords_int;
        std::vector< std::pair< SVector, std::string> > keywords_string;

        std::string name;
        std::string type;
        std::map<std::string, double> d_values;
        std::map<std::string, int> i_values;
        std::map<std::string, std::string> s_values;
    public:
        GenPropertyVisitor();

        void add_double_parameter(const SVector &v, double x, bool mandatory = false);
        void add_int_parameter(const SVector &v, int x, bool mandatory = false);

        void add_string_parameter(const SVector &v, std::string x, bool mandatory = false);

        // parses a property
        void operator()(const Property &p);
        // the user has to derive and write an operator to read a PropertyList
        void operator()(const PropertyList &pl); 
        
        bool check_mandatory() const;
        void setup_defaults();

        std::string get_name() const { return name; }
        std::string get_type() const { return type; }
        double get_dvalue(const std::string &n) const {return d_values.find(n)->second;}
        double get_ivalue(const std::string &n) const {return i_values.find(n)->second;}
        std::string get_svalue(const std::string &n) const {return s_values.find(n)->second;}
        
    };

    bool parse_properties(std::istream &is, const std::string &filename,
                          PropertyList &pset);
}

#endif

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
        std::map<std::string, double> d_values;
        std::map<std::string, int> i_values;

        typedef std::vector< std::string > SVector;
        typedef std::pair< SVector, double > SVDouble; 
        typedef std::pair< SVector, int > SVInt; 
        
        std::vector< std::string > mandatory_keys;
        std::vector< std::pair< SVector, double> > keywords_double;
        std::vector< std::pair< SVector, int> > keywords_int;

        std::string name;
        std::string type;
        
        /** search an array of SVectors, to see if a certain keyword exists */
        std::string check_keyword(const std::vector<SVDouble> &s, const std::string &name) const;
        /** search an array of SVectors, to see if a certain keyword exists */
        std::string check_keyword(const std::vector<SVInt> &s, const std::string &name) const;
    public:
        GenPropertyVisitor();

        void add_double_parameter(const SVector &v, double x, bool mandatory = false);
        void add_int_parameter(const SVector &v, int x, bool mandatory = false);

        // parses a property
        void operator()(const Property &p);
        // the user has to derive and write an operator to read a PropertyList
        void operator()(const PropertyList &pl); 
        
        bool check_mandatory() const;
    };

    bool parse_properties(std::istream &is, const std::string &filename,
        PropertyList &pset);
}

#endif

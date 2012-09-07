/*
 * taskset_parser.cpp
 *
 *  Created on: May 27, 2011
 *      Author: lipari
 */

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <models/task_model.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    Scan::Model::resource, 
    (std::string, name)
    (int, id)
    (bool, is_short)
    )

BOOST_FUSION_ADAPT_STRUCT(
    Scan::Model::cs_struct,
    (int, rid)
    (double, dur)
    (std::vector< boost::recursive_wrapper< Scan::Model::cs_struct> >, nested)
    )

BOOST_FUSION_ADAPT_STRUCT(
    Scan::Model::task_struct,
    (std::string, name)
    (double, w)
    (int, p)
    (double, d)
    (int, off)
    (std::vector< boost::recursive_wrapper< Scan::Model::cs_struct> >, nested)
    )

namespace Scan {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;

    using namespace Scan::Model;
    
    template <typename Iterator>
    struct taskres_grammar : qi::grammar<Iterator, sys_type(), ascii::space_type>
    {
        qi::rule<Iterator, cs_struct(), ascii::space_type> crit_sect;
        qi::rule<Iterator, std::vector<cs_struct>(), ascii::space_type> cs_list;
        qi::rule<Iterator, resource(), ascii::space_type> res;
        qi::rule<Iterator, task_struct(), ascii::space_type> task;
        qi::rule<Iterator, sys_elem(), ascii::space_type> elem;
        qi::rule<Iterator, sys_type(), ascii::space_type> sys;

        qi::rule<Iterator, double(), ascii::space_type> wcet;
        qi::rule<Iterator, double(), ascii::space_type> dline;
        qi::rule<Iterator, double(), ascii::space_type> dur;
        qi::rule<Iterator, int(), ascii::space_type> id;
        qi::rule<Iterator, int(), ascii::space_type> period;
        qi::rule<Iterator, int(), ascii::space_type> offset;
        qi::rule<Iterator, bool(), ascii::space_type> isshort;
        qi::rule<Iterator, std::string(), ascii::space_type> name;
        
        
        taskres_grammar() : taskres_grammar::base_type(sys, "System") {
            using qi::lit;
            using qi::int_;
            using qi::double_;
            using qi::bool_;
            using qi::int_;
            using qi::alpha;
            using qi::alnum;
            using qi::lexeme;
            
            wcet = (lit("w") | lit("wcet") | lit("C")) > '=' > double_;
            dline = (lit("d") | lit("dline") | lit("D")) > '=' > double_;
            dur = (lit("l") | lit("length") | lit("L")) > '=' > double_;
            
            id = lit("id") > '=' > int_;
            period = (lit("p") | "period" | "T") > '=' > int_;
            offset = (lit("o") | "offset" | "O") > '=' > int_;
            
            isshort = lit("short") > '=' > bool_;
            name =  lexeme[alpha >> *alnum];
            
            res = lit("res") > name > '{' > (id > isshort) > '}';
            
            cs_list = *crit_sect;
            crit_sect = lit("cs") > '{' > id > dur > -cs_list > '}';
            
            task = lit("task") > name > '{' > wcet > period > -dline > -offset > -cs_list > '}';

            elem = res | task;

            sys = lit("SYS") >> *elem >> lit("END"); 
        }
    };
}

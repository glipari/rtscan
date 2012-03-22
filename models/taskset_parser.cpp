/*
 * taskset_parser.cpp
 *
 *  Created on: May 27, 2011
 *      Author: lipari
 */

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <common/exceptions.hpp>
#include <models/taskset_parser.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    Scan::TaskModel::prop_struct,
    (std::string, name)
    (double, value)
    )

BOOST_FUSION_ADAPT_STRUCT(
    Scan::TaskModel::task_struct,
    (std::string, name)
    (std::vector< Scan::TaskModel::prop_struct >, properties)
    )

namespace Scan {
    namespace TaskModel {
        namespace qi = boost::spirit::qi;
        namespace ascii = boost::spirit::ascii;
        namespace fusion = boost::fusion;
        
        template <typename Iterator>
        struct taskset_grammar : qi::grammar<Iterator, task_struct(), //double()
                                             ascii::space_type>
        {
            taskset_grammar() : taskset_grammar::base_type(task)
                {
                    using qi::eps;
                    using qi::lit;
                    using qi::_val;
                    using qi::_1;
                    using ascii::alpha;
                    using ascii::alnum;
                    using qi::string;
                    using qi::double_;
                    using qi::lexeme;
                    
                    property_name %= (lexeme[alpha >> *alnum]);
                    property %= property_name >> lit('=') >> double_;
                    property_list %= property % ',';
                    task %= lit("task") >> (lexeme[alpha >> *alnum])
                                        >> '{' >> property_list >> '}';
                }
            qi::rule<Iterator, std::string(), ascii::space_type> property_name;
            qi::rule<Iterator, prop_struct() , ascii::space_type> property;
            qi::rule<Iterator, std::vector< prop_struct >() , ascii::space_type> property_list;
            qi::rule<Iterator, task_struct(), ascii::space_type> task;
        };
    }
    
    Scan::Task create_task(const TaskModel::task_struct &ts)
    {
	Scan::Task task(ts.name);
	std::vector< TaskModel::prop_struct >::const_iterator iter = ts.properties.begin();
	std::vector< TaskModel::prop_struct >::const_iterator end = ts.properties.end();
	int period = 0, wcet = 0, dline = 0;
	for (;iter != end; ++iter) {
            if (iter->name == "p") period = iter->value;
            else if (iter->name == "d") dline = iter->value;
            else if (iter->name == "c") wcet = iter->value;
	}
	task.set_period(period);
	if (dline > 0) task.set_dline(dline);
	else task.set_dline(period);
	task.set_wcet(wcet);

	return task;
    }
    
    Scan::TaskSet parse_tasks(const std::string &total)
    {
        typedef std::string::const_iterator iterator_type;
        typedef TaskModel::taskset_grammar<iterator_type> task_grammar;
        using boost::spirit::ascii::space;
        
        Scan::TaskSet ts;
        
        std::string::const_iterator iter = total.begin();
        std::string::const_iterator end = total.end();
        
        task_grammar grammar; // Our grammar
        
        while (iter != end) {
            TaskModel::task_struct result;
            bool r = phrase_parse(iter, end, grammar, space, result);
            
            if (r) {
                Scan::Task task = create_task(result);
                ts += task;
            } else throw ParseExc("Error in parsing the task set");
        }
        return ts;
    }
}

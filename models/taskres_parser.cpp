#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <iostream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <common/gen_parse.hpp>
#include <models/taskres_parser.hpp>

namespace Scan {
    namespace Model {
        struct resource {
            std::string name;
            int id;
            bool is_short;
        
            resource() : name(""), id(0), is_short(false) {}
        };

        struct cs_struct {
            int rid;
            double dur;
            std::vector< boost::recursive_wrapper<cs_struct> > nested;
        };
        
        struct task_struct {
            std::string name;
            double w;
            int p;
            double d;
            int off;
            std::vector< boost::recursive_wrapper<cs_struct> > nested;
            task_struct() : name(""), w(0), p(0), d(0), off(0) {}
        };
        
        typedef boost::variant<resource, task_struct> sys_elem; 
        typedef std::vector<sys_elem> sys_type;
    }
}

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

    struct SysVisitor : public boost::static_visitor<> {
        std::vector<TaskRes> tset;
        std::vector<Resource> rset;

        SysVisitor() {}
        
        void addNestedCS(CriticalSection &crit, const cs_struct &kk) {
            for (auto k : kk.nested) {
                CriticalSection cs(k.get().rid, k.get().dur);
                addNestedCS(cs, k.get());
                crit.addNestedCS(cs);
            }
        }

        void operator()(task_struct &ts) {
            // std::cout << "called the operator for task_struct!" << std::endl;
            // std::cout << "w = " << ts.w << std::endl;            
            // std::cout << "p = " << ts.p << std::endl;
            // std::cout << "d = " << ts.d << std::endl;            
            // std::cout << "off = " << ts.off << std::endl;
            // std::cout << "nested.size() = " << ts.nested.size() << std::endl;
            // for (unsigned i=0; i<ts.nested.size();++i) {
            //     cs_struct s = ts.nested[i].get();
            //     std::cout << "nested[" << i << "].nested.size() = " << s.nested.size() << std::endl;
            // }
            if (ts.d == 0) 
                ts.d = ts.p;
            TaskRes task(ts.w , ts.d, ts.p , ts.off);
            // now the critical sections
            for (auto k : ts.nested) {
                CriticalSection cs(k.get().rid, k.get().dur);
                addNestedCS(cs, k.get());
                task.addCS(cs);
            }
            tset.push_back(task);
        }

        void operator()(const resource &rs) {
            Resource res(rs.id, rs.is_short);
            rset.push_back(res);
        }
    };
    
    bool parse_taskres(const std::string &total, std::vector<TaskRes> &tset, std::vector<Resource> &rset)
    {
        std::stringstream str(total);
        sys_type system; 
        bool r;
        try {
            r = myparse<taskres_grammar, sys_type>(str, "internal_file_name", system);
            SysVisitor vis;
            for (auto s : system) boost::apply_visitor(std::ref(vis), s); // convert system
            tset = vis.tset;
            rset = vis.rset;
            return true;
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Run time error: " << e.what() << std::endl;
            return false;
        }
        catch (const std::exception& e)
        {
            std::cerr << "An exception has been thrown: " << e.what() << std::endl;
            return false;
        }
    }

    void parse_taskres(std::istream &is, const std::string &filename, std::vector<TaskRes> &tset, std::vector<Resource> &rset)
    {
        sys_type system;
        if (myparse<taskres_grammar, sys_type>(is, filename, system)) {
            assert(system.size() > 0);
            SysVisitor vis;
            for (auto s : system) boost::apply_visitor(std::ref(vis), s); // convert system
            tset = vis.tset;
            rset = vis.rset;
        }
    }
}

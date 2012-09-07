#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <iostream>

#include <common/gen_parse.hpp>
#include <models/taskres_parser.hpp>
#include <models/taskset_parser.hpp>

namespace Scan {
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

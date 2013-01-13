#include <models/computational_resource.hpp>
namespace Scan {

	std::ostream & operator<<(std::ostream &o, FPTask t)
        {
                o << t.get_name() << "  " << t.get_node() << "  " << t.get_wcet() << "  " << t.get_dline() << "  " << t.get_period() <<"  "<< t.get_sched() << "  " << t.get_priority();
                return o;
        }


}

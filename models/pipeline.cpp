#include <models/pipeline.hpp>

namespace Scan {

	int Pipeline::counter = 0;

    Pipeline::Pipeline(const Pipeline &other) :
        period(other.period), e2e_dline(other.e2e_dline),
                            name(other.name), tag(other.tag) {
            for ( auto x : other.pline_tasks )
                pline_tasks.push_back(x);
    }


	bool Pipeline::register_a_fp_task(FPTask_ptr task) {
		task->set_period(period);
		task->set_dline(e2e_dline);
		task->set_pipeline_tag(tag);
		pline_tasks.push_back(task);
		sort(pline_tasks.begin(), pline_tasks.end(), PlineOps());
        return true;
	}
	
	std::ostream & operator<<(std::ostream &o, Pipeline &pline) {
		o << "pipeline name : " << pline.get_name() << ", period: ";
        o << pline.get_period() << ", e2edline: " << pline.get_e2e_dline();
        o << " tasks: ";
		for( auto x : pline.pline_tasks)
			o<< "  " << x->get_name();
		o<<std::endl;
	}
}

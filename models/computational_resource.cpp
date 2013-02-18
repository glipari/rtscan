#include <models/computational_resource.hpp>
namespace Scan {

	bool ComputationalResource::register_a_fp_task(FPTask_ptr task) {
		const std::string policy = task->get_sched();

		if( policy=="fp") {
			rq_fp.push_back(task);	
			sort(rq_fp.begin(), rq_fp.end(), NodeOps());
			rq.push_back(task);	
			sort(rq.begin(), rq.end(), NodeOps());
			return true;
		}
		else if( policy=="npfp") {
			rq_npfp.push_back(task);	
			sort(rq_npfp.begin(), rq_npfp.end(), NodeOps());
			rq.push_back(task);	
			sort(rq.begin(), rq.end(), NodeOps());
			return true;
		}
		throw ("Not a valid scheduling policy\n");
	}

	std::vector<FPTask_ptr> ComputationalResource::get_rq_fp() const 
	{ return rq_fp; }
	std::vector<FPTask_ptr> ComputationalResource::get_rq_npfp() const 
	{ return rq_npfp; }
	std::vector<FPTask_ptr> ComputationalResource::get_rq() const 
	{ return rq; }

}

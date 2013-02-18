#ifndef __COMPUTATIONAL_RESOURCE_H
#define __COMPUTATIONAL_RESOURCE_H

#include <algorithm>
#include <vector>
#include <models/fp_task.hpp>

namespace Scan {
	/**
	  An instance of ComputationalResource could be a CPU and 
	  a network, .etc.
	*/
	class ComputationalResource {
		struct NodeOps {
			bool operator () 
				(const FPTask_ptr &a, const FPTask_ptr &b)
			{
				return (a->get_priority() > b->get_priority());
			}
		};
	public :
        /** All fp tasks in this node are stored in this runqueue. */
		std::vector<FPTask_ptr> rq_fp;
        /** 
            All fully-preemptive fp tasks in this node are stored here. 
            Tasks are ordered by higher priority first.
        */
		std::vector<FPTask_ptr> rq;
        /** 
            All non-preemptive fp tasks in this node are stored here.
            Tasks are ordered by higher priority first.
        */
		std::vector<FPTask_ptr> rq_npfp;

		bool register_a_fp_task(FPTask_ptr task);
		std::vector<FPTask_ptr> get_rq_fp() const;
		std::vector<FPTask_ptr> get_rq_npfp() const;
		std::vector<FPTask_ptr> get_rq() const;

	};

	//std::ostream & operator<<(std::ostream &o, FPTask t);

}

#endif

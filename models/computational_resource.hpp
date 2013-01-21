#ifndef __COMPUTATIONAL_RESOURCE_H
#define __COMPUTATIONAL_RESOURCE_H

#include <algorithm>
#include <vector>
#include <models/fp_task.hpp>

namespace Scan {
	/**
	 * An instance of ComputationalResource could be a CPU and 
	 * a network, .etc.
	 **/
	class ComputationalResource {
		struct comp_prio {
			bool operator ()
				(FPTask task1, FPTask task2)
			{
				return 
				(task1.get_priority() > task2.get_priority());
			}
		} comp_obj;
	public :
		/**
		 * The vector v_fpfp is used to register 
		 * fixed-priority fully-preemptive tasks.
		 * Tasks are ordered from higher priority 
		 * to lower priority inside the vector.
		 **/
		std::vector<FPTask> v_fpfp;
		/**
		 * The vector v_fpnp is used to register 
		 * fixed-priority non-preemptive tasks.
		 * Tasks are ordered from higher priority 
		 * to lower priority inside the vector.
		 **/
		std::vector<FPTask> v_fpnp;

		std::vector<FPTask> v_tasks;

		bool register_a_fp_task(FPTask task) {
			const std::string policy = task.get_sched();
			//std::cout<<"scheduling policy : "<<policy<<std::endl;
			if( policy.compare("fp") == 0) {
				v_fpfp.push_back(task);	
				sort(v_fpfp.begin(), v_fpfp.end(), comp_obj);
				v_tasks.push_back(task);	
				sort(v_tasks.begin(), v_tasks.end(), comp_obj);
				return true;
			}
			else if( policy.compare("npfp") == 0) {
				v_fpnp.push_back(task);	
				sort(v_fpnp.begin(), v_fpnp.end(), comp_obj);
				v_tasks.push_back(task);	
				sort(v_tasks.begin(), v_tasks.end(), comp_obj);
				return true;
			}
			return false;
		}

		int get_nvars() {
			int nvars = 0;
			for ( std::vector<FPTask>::iterator i = v_tasks.begin();
					i != v_tasks.end(); ++i)
				nvars += 3;
			return nvars;

			for ( std::vector<FPTask>::iterator i = v_tasks.begin();
					i != v_tasks.end(); ++i)
				if( i->get_pipeline_pos() > 1)
					nvars += 3;
				else if (i->get_pipeline_pos() == 1)
					nvars += 2;	
				else nvars += 1;
/*
			for ( std::vector<FPTask>::iterator i = v_fpfp.begin();
					i != v_fpfp.end(); ++i) {
				if( i->get_pipeline_pos() >= 1)
					nvars += 3;
				else nvars += 1;
			}
			for ( std::vector<FPTask>::iterator i = v_fpnp.begin();
					i != v_fpnp.end(); ++i) {
				if( i->get_pipeline_pos() >= 1)
					nvars += 3;
				else nvars += 1;
			}
*/			return nvars;
		}
	};

	std::ostream & operator<<(std::ostream &o, FPTask t);
	//{
	//	o << t.get_wcet() << "  " << t.get_dline() << "  " << t.get_period() <<"  "<< t.get_sched() << "  " << t.get_priority();
	//	return o;
//	}

}

#endif

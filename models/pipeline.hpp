#ifndef __PIPELINE_H
#define __PIPELINE_H

#include <vector>
#include <models/fp_task.hpp>

namespace Scan {

	class Pipeline {
	public :
		std::vector<FPTask> tasks;
		int period;
		int e2e_dline;
		std::string name;
		
		Pipeline() : period(0), e2e_dline(0) {}
		Pipeline(const int _period, const int _dline) :
			period(_period), e2e_dline(_dline) {}
		Pipeline(const Pipeline &other) :
			period(other.period), e2e_dline(other.e2e_dline),
				name(other.name), tasks(other.tasks) {}

		void set_period(int _period) { period = _period; }
		int get_period() { return period; }
		int get_e2e_dline() { return e2e_dline; }
		void set_e2e_dline(int dline) { e2e_dline = dline; }

		void set_pipeline_param(FPTask &task) {
			task.set_period(period);
			task.set_dline(e2e_dline);
			if( task.get_pipeline_pos() == 1)
				task.set_jitter(0);
		}	

		void register_a_fp_task(FPTask task) {
			tasks.push_back(task);
		}
		
		void set_name(std::string n) { name = n;}
		std::string get_name() { return name; }
	};
}

#endif

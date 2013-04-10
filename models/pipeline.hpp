#ifndef __PIPELINE_H
#define __PIPELINE_H

#include <vector>
#include <models/fp_task.hpp>
#include <models/has_unique_id.hpp>
#include <algorithm>

namespace Scan {

	class Pipeline {
		static int counter;
		struct PlineOps {
			bool operator()
				(const FPTask_ptr &a, const FPTask_ptr &b)	
			{	
                return (a->get_pipeline_pos() < b->get_pipeline_pos()); 
            }
		};
	public :
        /** 
            Tasks in this pipeline, ordered according to their positions
            in the pipeline.
        */
		std::vector<FPTask_ptr> pline_tasks;
        /** The period of the pipeline. */
		int period;
        /** The end-2-end deadline of the pipeline. */
		int e2e_dline;
        /** The name of the pipeline. */
		std::string name;
        /** The tag of the pipeline. */
		int tag;
		
		Pipeline() : period(0), e2e_dline(0) {tag = counter++;}

		Pipeline(const int _period, const int _dline) :
			period(_period), e2e_dline(_dline) {tag=counter++;}

		Pipeline(const Pipeline &other);
//			period(other.period), e2e_dline(other.e2e_dline), 
//                                        name(other.name), tag(other.tag) {
//			for ( auto x : other.pline_tasks )
//				pline_tasks.push_back(x);	
//		}

		void set_period(int _period) { period = _period; }

		int get_period() const { return period; }

		void set_e2e_dline(int dline) { e2e_dline = dline; }

		int get_e2e_dline() const { return e2e_dline; }


		bool register_a_fp_task(FPTask_ptr task);
		
		void set_name(std::string n) { name = n;}

		std::string get_name() const { return name; }

		int get_tag() const { return tag; }
	};

	std::ostream & operator<<(std::ostream &o, Pipeline & pline);
}

#endif

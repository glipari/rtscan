#ifndef __PIPELINE_PROPERTY_PARSER_HPP_
#define __PIPELINE_PROPERTY_PARSER_HPP_

#include <common/property_parser.hpp>
#include <models/pipeline.hpp>

namespace Scan {

	class PipelinePropVisitor : public GenPropertyVisitor {
	public:
		PipelinePropVisitor();
		Pipeline create_pipeline();
	};
	Pipeline create_pipeline(const PropertyList &p);

}

#endif

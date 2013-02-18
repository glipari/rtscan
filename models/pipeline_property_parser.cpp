#include <models/pipeline_property_parser.hpp>

namespace Scan {

	PipelinePropVisitor::PipelinePropVisitor()
	{
		add_int_parameter({"Period", "T", "p"}, 0, true);
		add_int_parameter({"E2Edline", "D", "e"}, 0, true);
	}

	Pipeline PipelinePropVisitor::create_pipeline() {
		if (!check_mandatory()) THROW_EXC(IllegalValue, "Mandatory parameter is missing");
		if (d_values["E2Edline"] < 0.0)
			d_values["E2Edline"] = i_values["Period"];
		Pipeline pipe(i_values["Period"], i_values["E2Edline"]);
		pipe.set_name(name);
		return pipe;
	}

	Pipeline create_pipeline(const PropertyList &p)
	{
		if( p.type != "pipeline") 
			 THROW_EXC(IncorrectType, 
					"Expecting a pipeline type");
		Pipeline pipe;
		pipe.set_name(p.name);
		return pipe;

		PipelinePropVisitor vis;
		vis(p);
		return vis.create_pipeline();
	}

}

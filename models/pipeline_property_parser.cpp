#include <models/pipeline_property_parser.hpp>

namespace Scan {

	PipelinePropVisitor::PipelinePropVisitor()
	{
		add_int_parameter({"period", "T", "p"}, 0, true);
		add_int_parameter({"e2edline", "D", "e"}, 0, true);
	}

	Pipeline PipelinePropVisitor::create_pipeline() {
		if (!check_mandatory()) THROW_EXC(IllegalValue, "Mandatory parameter is missing");
		if (d_values["dline"] < 0.0)
			d_values["e2edline"] = i_values["period"];
		Pipeline pipe(i_values["period"], i_values["e2edline"]);
		pipe.set_name(name);
		return pipe;
	}

	Pipeline create_pipeline(const PropertyList &p)
	{
		if( p.type != "pipelineprop") 
			 THROW_EXC(IncorrectType, 
					"Expecting a pipeline property type");
		PipelinePropVisitor vis;
		vis(p);
		return vis.create_pipeline();
	}

}

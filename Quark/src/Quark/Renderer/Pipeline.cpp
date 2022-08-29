#include "qkpch.h"
#include "Pipeline.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		return s_GraphicsAPI->CreatePipeline(spec);
	}
}

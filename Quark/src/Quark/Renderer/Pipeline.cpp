#include "qkpch.h"
#include "Pipeline.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		return GraphicsAPI::Instance->CreatePipeline(spec);
	}
}

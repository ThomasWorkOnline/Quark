#include "qkpch.h"
#include "RenderPipeline.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<RenderPipeline> RenderPipeline::Create()
	{
		return GraphicsAPI::Instance->CreateRenderPipeline();
	}
}

#include "qkpch.h"
#include "RenderPass.h"

#include "GraphicsAPI.h"

namespace Quark {

	Scope<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateRenderPass(spec);
	}
}

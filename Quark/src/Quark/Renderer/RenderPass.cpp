#include "qkpch.h"
#include "RenderPass.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Scope<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		return s_GraphicsAPI->CreateRenderPass(spec);
	}
}

#include "qkpch.h"
#include "RenderPass.h"

#include "GraphicsAPI.h"

namespace Quark {

	RenderPass* RenderPass::Create(const RenderPassSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateRenderPass(spec);
	}
}

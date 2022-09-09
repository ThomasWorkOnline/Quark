#include "qkpch.h"
#include "OpenGLRenderPass.h"

namespace Quark {

	OpenGLRenderPass::OpenGLRenderPass(const RenderPassSpecification& spec)
		: RenderPass(spec)
	{
	}

	bool OpenGLRenderPass::operator==(const RenderPass& other) const
	{
		return false;
	}
}

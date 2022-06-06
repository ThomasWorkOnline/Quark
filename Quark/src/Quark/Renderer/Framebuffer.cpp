#include "qkpch.h"
#include "Framebuffer.h"

#include "RenderCommand.h"

namespace Quark {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return RenderCommand::CreateFramebuffer(spec);
	}
}

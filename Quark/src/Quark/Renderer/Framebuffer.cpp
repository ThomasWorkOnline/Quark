#include "qkpch.h"
#include "Framebuffer.h"

#include "GraphicsAPI.h"

namespace Quark {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateFramebuffer(spec);
	}
}

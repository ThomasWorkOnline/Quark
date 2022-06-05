#include "qkpch.h"
#include "Framebuffer.h"

#include "GraphicsAPI.h"

// Include all supported API's frame buffer implementations
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Quark {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLFramebuffer>(spec);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}

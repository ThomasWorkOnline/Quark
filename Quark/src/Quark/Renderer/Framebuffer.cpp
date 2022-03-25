#include "Framebuffer.h"

#include "RenderingAPI.h"

// Include all supported API's frame buffer implementations
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Quark {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLFramebuffer>(spec);
			default:
				QK_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}

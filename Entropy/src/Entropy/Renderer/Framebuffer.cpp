#include "Framebuffer.h"

#include "RenderingAPI.h"

// Include all supported API's frame buffer implementations
#include "../../Platform/OpenGL/OpenGLFramebuffer.h"

namespace Entropy {

	Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height, uint32_t samples, bool swapChainTarget)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			return CreateRef<OpenGLFramebuffer>(width, height, samples, swapChainTarget);
		case RenderingAPI::API::None:
			NT_FATAL("Rendering API not supported");
			return nullptr;
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}
}

#include "Cubemap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "Platform/OpenGL/OpenGLCubemap.h"

namespace Quark {

	Ref<Cubemap> Cubemap::Create(uint32_t width, uint32_t height)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLCubemap>(width, height);
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}

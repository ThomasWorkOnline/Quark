#include "CubeMap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "Platform/OpenGL/OpenGLCubeMap.h"

namespace Quark {

	Ref<CubeMap> Quark::CubeMap::Create(uint32_t width, uint32_t height)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLCubeMap>(width, height);
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}

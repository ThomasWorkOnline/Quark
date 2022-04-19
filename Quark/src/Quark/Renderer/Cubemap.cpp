#include "Cubemap.h"

#include "RenderingAPI.h"

// Include all supported API's environment maps implementations
#include "Platform/OpenGL/OpenGLCubemap.h"

namespace Quark {

	Ref<Cubemap> Cubemap::Create(const CubemapSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPI::API::OpenGL:
				return CreateRef<OpenGLCubemap>(spec);
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}

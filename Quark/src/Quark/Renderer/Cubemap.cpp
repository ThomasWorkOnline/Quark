#include "qkpch.h"
#include "Cubemap.h"

#include "GraphicsAPI.h"

// Include all supported API's environment maps implementations
#include "Platform/OpenGL/OpenGLCubemap.h"

namespace Quark {

	Ref<Cubemap> Cubemap::Create(const CubemapSpecification& spec)
	{
		switch (GraphicsAPI::GetAPI())
		{
			case GraphicsAPI::API::OpenGL:
				return CreateRef<OpenGLCubemap>(spec);
			default:
				QK_CORE_FATAL("Graphics API not supported");
				return nullptr;
		}
	}
}

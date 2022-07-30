#include "qkpch.h"
#include "GraphicsAPI.h"

#define USE_VULKAN 0

namespace Quark {

	API GetDefaultAPIForPlatform()
	{
#if USE_VULKAN
		return API::Vulkan;
#else
		return API::OpenGL; // TODO: DirectX
#endif
	}
}

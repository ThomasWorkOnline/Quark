#include "qkpch.h"
#include "GraphicsAPI.h"

#define USE_VULKAN 0

namespace Quark {

	RHI GetDefaultRHIForPlatform()
	{
#if USE_VULKAN
		return RHI::Vulkan;
#else
		return RHI::OpenGL; // TODO: DirectX
#endif
	}
}

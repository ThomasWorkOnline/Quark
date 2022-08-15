#include "qkpch.h"
#include "GraphicsAPI.h"

namespace Quark {

	RHI GetDefaultRHIForPlatform()
	{
		return RHI::Vulkan;
	}
}

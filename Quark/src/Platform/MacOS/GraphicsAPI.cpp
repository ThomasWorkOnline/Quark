#include "qkpch.h"
#include "GraphicsAPI.h"

#include "Metal/MetalGraphicsAPI.h"

namespace Quark {

	RHI GetDefaultRHIForPlatform()
	{
		return RHI::Metal;
	}

	Scope<GraphicsAPI> CreateDefaultRHIForPlatform()
	{
		return CreateScope<MetalGraphicsAPI>();
	}
}

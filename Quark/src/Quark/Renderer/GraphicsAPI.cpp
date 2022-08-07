#include "qkpch.h"
#include "GraphicsAPI.h"

// Include all supported APIs here
#ifdef QK_PLATFORM_APPLE
#	include "Platform/MacOS/Metal/MetalGraphicsAPI.h"
#endif

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

namespace Quark {

	API GraphicsAPI::s_API = API::None;
	Scope<GraphicsAPI> GraphicsAPI::Instance = Instantiate(GetDefaultForPlatform());

	Scope<GraphicsAPI> GraphicsAPI::Instantiate(API api)
	{
		s_API = api;
		switch (api)
		{
#ifdef QK_PLATFORM_APPLE
			case API::Metal:  return CreateScope<MetalGraphicsAPI>();
#endif
			case API::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case API::Vulkan: return CreateScope<VulkanGraphicsAPI>();

			QK_ASSERT_DEFAULT("Unknown graphics API", nullptr);
		}
	}
}

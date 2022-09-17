#include "qkpch.h"

#include "Quark/Renderer/GraphicsAPI.h"

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/Vulkan/VulkanGraphicsAPI.h"

namespace Quark {

	RHI GraphicsAPI::GetDefaultRHIForPlatform()
	{
		return RHI::Vulkan;
	}

	Scope<GraphicsAPI> GraphicsAPI::CreateRHIForPlatform(RHI api)
	{
		switch (api)
		{
			case RHI::OpenGL: return CreateScope<OpenGLGraphicsAPI>();
			case RHI::Vulkan: return CreateScope<VulkanGraphicsAPI>();

			QK_ASSERT_NO_DEFAULT("Graphics API is not supported!");
		}

		return nullptr;
	}
}

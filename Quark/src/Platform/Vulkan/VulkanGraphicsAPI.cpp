#include "qkpch.h"
#include "VulkanGraphicsAPI.h"

#include "VulkanGraphicsContext.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	VulkanGraphicsAPI::~VulkanGraphicsAPI()
	{
		QK_PROFILE_FUNCTION();
	}

	void VulkanGraphicsAPI::Init()
	{
		QK_PROFILE_FUNCTION();
	}

	Scope<GraphicsContext> VulkanGraphicsAPI::CreateGraphicsContext(void* windowHandle)
	{
		return CreateScope<VulkanGraphicsContext>(windowHandle);
	}
}

#pragma once

#include "Quark/Core/Core.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanRenderer : public StaticClass
	{
	public:
		static void Initialize();
		static void Shutdown();

		static VkFence     GetInFlightFence();
		static VkSemaphore GetRenderFinishedSemaphore();
		static VkSemaphore GetImageAvailableSemaphore();
	};
}

#pragma once

#include "Quark/Core/Core.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	namespace Utils {

		void                     EnumerateVkExtensions();

		VkBuffer                 AllocateBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
		void                     CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size);
	}
}

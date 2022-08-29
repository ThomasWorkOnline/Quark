#pragma once

#include "Quark/Core/Core.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	namespace Utils {

		void EnumerateVkExtensions();

		QK_NODISCARD uint32_t GetBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		QK_NODISCARD VkBuffer AllocateBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, size_t* outRequirementSize = nullptr);
		QK_NODISCARD VkImage  AllocateImage(VulkanDevice* device, VkExtent3D extent, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, size_t* outRequirementSize = nullptr);

		void CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size);
		void CopyBufferToImage(VulkanDevice* device, VkImage dstImage, VkBuffer srcBuffer, VkExtent3D extent);
	}
}

#pragma once

#include "Quark/Core/Core.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	namespace Utils {

		void EnumerateVkExtensions();

		uint32_t GetBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkBuffer AllocateBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, size_t* outRequirementSize = nullptr);
		VkImage  AllocateImage(VulkanDevice* device, VkExtent3D extent, uint32_t layers, uint32_t levels, uint32_t samples, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, size_t* outRequirementSize = nullptr);

		void CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size, size_t srcOffset, size_t dstOffset);
		void CopyBufferToImage(VulkanDevice* device, VkImage dstImage, VkBuffer srcBuffer, VkExtent3D extent, uint32_t layers, uint32_t levels);
	}
}

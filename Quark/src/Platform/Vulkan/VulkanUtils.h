#pragma once

#include "Quark/Core/Core.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	namespace Utils {

		void EnumerateVkExtensions();

		uint32_t GetBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkMemoryRequirements AllocateBuffer(VulkanDevice* device, VkDeviceSize size,
			                    VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

		VkMemoryRequirements AllocateImage(VulkanDevice* device, VkExtent3D extent, uint32_t layerCount, uint32_t levels, uint32_t samples, VkFormat format,
			                  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* bufferMemory);

		void AllocateImageView(VulkanDevice* device, VkImage image, VkFormat format, uint32_t layerCount, uint32_t levelCount, VkImageView* imageView);

		void CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size, size_t srcOffset, size_t dstOffset);

		void ClearImage(VulkanDevice* device, VkImage dstImage, VkImageLayout layout, VkClearColorValue clearColor, uint32_t layerCount, uint32_t baseLayer, uint32_t levelCount);

		void CopyBufferToImage(VulkanDevice* device, VkBuffer srcBuffer, VkDeviceSize bufferOffset,
			                    VkImage dstImage, VkExtent3D imageExtent, VkOffset3D imageOffset, uint32_t layerCount, uint32_t baseLayer, uint32_t levelCount);

		void CopyImageToBuffer(VulkanDevice* device, VkImage srcImage, VkExtent3D imageExtent, VkOffset3D imageOffset, uint32_t layerCount, uint32_t baseLayer, uint32_t level,
			                   VkBuffer dstBuffer, VkDeviceSize bufferOffset);

		void GenerateMipmaps(VulkanDevice* device, VkImage image, uint32_t width, uint32_t height, uint32_t levelCount);

		void TransitionImageLayout(VulkanDevice* device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t levelCount);
	}
}

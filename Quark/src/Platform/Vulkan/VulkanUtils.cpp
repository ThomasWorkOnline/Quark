#include "qkpch.h"
#include "VulkanUtils.h"

#include "VulkanEnums.h"

namespace Quark {

	namespace Utils {

		void EnumerateVkExtensions()
		{
			uint32_t extensionCount = 0;
			VkResult vkRes = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan extension properties");

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkRes = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Error enumerating Vulkan extension properties");

			std::stringstream ss;
			ss << "available extensions:\n";
			for (const auto& extension : extensions)
				ss << '\t' << extension.extensionName << '\n';

			QK_CORE_INFO(ss.str());
		}

		uint32_t GetBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			QK_CORE_ASSERT(false, "Unable to find suitable memory type for buffer");
			return 0;
		}

		VkBuffer AllocateBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, size_t* outRequirementSize)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			auto vkPhysicalDevice = device->GetPhysicalDevice();

			VkBuffer buffer;
			vkCreateBuffer(device->GetVkHandle(), &bufferInfo, nullptr, &buffer);

			VkMemoryRequirements req;
			vkGetBufferMemoryRequirements(device->GetVkHandle(), buffer, &req);

			if (outRequirementSize) *outRequirementSize = req.size;

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = req.size;
			allocInfo.memoryTypeIndex = GetBufferMemoryType(vkPhysicalDevice, req.memoryTypeBits, properties);

			vkAllocateMemory(device->GetVkHandle(), &allocInfo, nullptr, &bufferMemory);
			vkBindBufferMemory(device->GetVkHandle(), buffer, bufferMemory, 0);

			return buffer;
		}

		VkImage AllocateImage(VulkanDevice* device, VkExtent3D extent, uint32_t layers, uint32_t levels, uint32_t samples, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory, size_t* outRequirementSize)
		{
			VkImageCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			createInfo.imageType = VK_IMAGE_TYPE_2D;
			createInfo.format = format;
			createInfo.extent = extent;
			createInfo.mipLevels = levels;
			createInfo.arrayLayers = layers;
			createInfo.samples = SampleCountToVulkan(samples);
			createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			createInfo.usage = usage;
			createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			
			VkImage image;
			vkCreateImage(device->GetVkHandle(), &createInfo, nullptr, &image);

			VkMemoryRequirements req;
			vkGetImageMemoryRequirements(device->GetVkHandle(), image, &req);

			if (outRequirementSize) *outRequirementSize = req.size;

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = req.size;
			allocInfo.memoryTypeIndex = GetBufferMemoryType(device->GetPhysicalDevice(), req.memoryTypeBits, properties);

			vkAllocateMemory(device->GetVkHandle(), &allocInfo, nullptr, &bufferMemory);
			vkBindImageMemory(device->GetVkHandle(), image, bufferMemory, 0);

			return image;
		}

		void CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size, size_t srcOffset, size_t dstOffset)
		{
			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = srcOffset;
			copyRegion.dstOffset = dstOffset;
			copyRegion.size = size;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			VkQueue transferQueue = device->GetTransferQueue();
			vkQueueSubmit(transferQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(transferQueue);
		}

		void CopyBufferToImage(VulkanDevice* device, VkImage dstImage, VkBuffer srcBuffer, VkExtent3D extent, uint32_t layers, uint32_t levels)
		{
			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkImageMemoryBarrier copyBarrier{};
			copyBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copyBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			copyBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			copyBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copyBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copyBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copyBarrier.image = dstImage;
			copyBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyBarrier.subresourceRange.levelCount = levels;
			copyBarrier.subresourceRange.layerCount = layers;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copyBarrier);

			VkBufferImageCopy region{};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.layerCount = layers;
			region.imageExtent = extent;

			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VkImageMemoryBarrier useBarrier{};
			useBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			useBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			useBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			useBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			useBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			useBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			useBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			useBarrier.image = dstImage;
			useBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			useBarrier.subresourceRange.levelCount = levels;
			useBarrier.subresourceRange.layerCount = layers;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &useBarrier);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			VkQueue transferQueue = device->GetTransferQueue();
			vkQueueSubmit(transferQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(transferQueue);
		}
	}
}

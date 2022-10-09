#include "qkpch.h"
#include "VulkanUtils.h"

#include "VulkanEnums.h"

namespace Quark {

	namespace Utils {

		void EnumerateVkExtensions()
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

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

		VkMemoryRequirements AllocateBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size        = size;
			bufferInfo.usage       = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			auto vkPhysicalDevice = device->GetPhysicalDevice();

			vkCreateBuffer(device->GetVkHandle(), &bufferInfo, nullptr, buffer);

			VkMemoryRequirements req;
			vkGetBufferMemoryRequirements(device->GetVkHandle(), *buffer, &req);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = req.size;
			allocInfo.memoryTypeIndex = GetBufferMemoryType(vkPhysicalDevice, req.memoryTypeBits, properties);

			vkAllocateMemory(device->GetVkHandle(), &allocInfo, nullptr, bufferMemory);
			vkBindBufferMemory(device->GetVkHandle(), *buffer, *bufferMemory, 0);

			return req;
		}

		VkMemoryRequirements AllocateImage(VulkanDevice* device, VkExtent3D extent, uint32_t layerCount, uint32_t levels, uint32_t samples, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* bufferMemory)
		{
			QK_PROFILE_FUNCTION();

			VkImageCreateInfo createInfo{};
			createInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			createInfo.imageType     = VK_IMAGE_TYPE_2D;
			createInfo.format        = format;
			createInfo.extent        = extent;
			createInfo.mipLevels     = levels;
			createInfo.arrayLayers   = layerCount;
			createInfo.samples       = SampleCountToVulkan(samples);
			createInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
			createInfo.usage         = usage;
			createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
			
			vkCreateImage(device->GetVkHandle(), &createInfo, nullptr, image);

			VkMemoryRequirements req;
			vkGetImageMemoryRequirements(device->GetVkHandle(), *image, &req);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = req.size;
			allocInfo.memoryTypeIndex = GetBufferMemoryType(device->GetPhysicalDevice(), req.memoryTypeBits, properties);

			vkAllocateMemory(device->GetVkHandle(), &allocInfo, nullptr, bufferMemory);
			vkBindImageMemory(device->GetVkHandle(), *image, *bufferMemory, 0);

			return req;
		}

		void AllocateImageView(VulkanDevice* device, VkImage image, VkFormat format, uint32_t layerCount, uint32_t levelCount, VkImageView* imageView)
		{
			QK_PROFILE_FUNCTION();

			VkComponentMapping componentsSwizzle{};
			componentsSwizzle.r = VK_COMPONENT_SWIZZLE_R;
			componentsSwizzle.g = VK_COMPONENT_SWIZZLE_G;
			componentsSwizzle.b = VK_COMPONENT_SWIZZLE_B;
			componentsSwizzle.a = VK_COMPONENT_SWIZZLE_A;

			VkImageViewCreateInfo createInfo{};
			createInfo.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image                       = image;
			createInfo.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format                      = format;
			createInfo.components                  = componentsSwizzle;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.levelCount = levelCount;
			createInfo.subresourceRange.layerCount = layerCount;

			vkCreateImageView(device->GetVkHandle(), &createInfo, nullptr, imageView);
		}

		void CopyBuffer(VulkanDevice* device, VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size, size_t srcOffset, size_t dstOffset)
		{
			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = srcOffset;
			copyRegion.dstOffset = dstOffset;
			copyRegion.size      = size;
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

		void ClearImage(VulkanDevice* device, VkImage dstImage, VkImageLayout layout, VkClearColorValue clearColor, uint32_t layerCount, uint32_t baseLayer, uint32_t levelCount)
		{
			QK_PROFILE_FUNCTION();

			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkImageSubresourceRange range{};
			range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			range.levelCount     = levelCount;
			range.baseArrayLayer = baseLayer;
			range.layerCount     = layerCount;
			vkCmdClearColorImage(commandBuffer, dstImage, layout, &clearColor, 1, &range);

			VkImageMemoryBarrier useBarrier{};
			useBarrier.sType                       = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			useBarrier.srcAccessMask               = VK_ACCESS_TRANSFER_WRITE_BIT;
			useBarrier.dstAccessMask               = VK_ACCESS_SHADER_READ_BIT;
			useBarrier.oldLayout                   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			useBarrier.newLayout                   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			useBarrier.srcQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
			useBarrier.dstQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
			useBarrier.image                       = dstImage;
			useBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			useBarrier.subresourceRange.levelCount = levelCount;
			useBarrier.subresourceRange.layerCount = layerCount;
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

		void CopyBufferToImage(VulkanDevice* device, VkBuffer srcBuffer, VkDeviceSize bufferOffset,
			                   VkImage dstImage, VkExtent3D imageExtent, VkOffset3D imageOffset, uint32_t layerCount, uint32_t baseLayer, uint32_t levelCount)
		{
			QK_PROFILE_FUNCTION();

			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkImageMemoryBarrier copyBarrier{};
			copyBarrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copyBarrier.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
			copyBarrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
			copyBarrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copyBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			copyBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			copyBarrier.image                           = dstImage;
			copyBarrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			copyBarrier.subresourceRange.baseArrayLayer = baseLayer;
			copyBarrier.subresourceRange.levelCount     = levelCount;
			copyBarrier.subresourceRange.layerCount     = layerCount;
			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copyBarrier);

			VkBufferImageCopy region{};
			region.bufferOffset                         = bufferOffset;
			region.imageSubresource.aspectMask          = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.baseArrayLayer      = baseLayer;
			region.imageSubresource.layerCount          = layerCount;
			region.imageOffset                          = imageOffset;
			region.imageExtent                          = imageExtent;
			vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VkImageMemoryBarrier useBarrier{};
			useBarrier.sType                            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			useBarrier.srcAccessMask                    = VK_ACCESS_TRANSFER_WRITE_BIT;
			useBarrier.dstAccessMask                    = VK_ACCESS_SHADER_READ_BIT;
			useBarrier.oldLayout                        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			useBarrier.newLayout                        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			useBarrier.srcQueueFamilyIndex              = VK_QUEUE_FAMILY_IGNORED;
			useBarrier.dstQueueFamilyIndex              = VK_QUEUE_FAMILY_IGNORED;
			useBarrier.image                            = dstImage;
			useBarrier.subresourceRange.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
			useBarrier.subresourceRange.baseArrayLayer  = baseLayer;
			useBarrier.subresourceRange.levelCount      = levelCount;
			useBarrier.subresourceRange.layerCount      = layerCount;
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

		void CopyImageToBuffer(VulkanDevice* device, VkImage srcImage, VkExtent3D imageExtent, VkOffset3D imageOffset, uint32_t layerCount, uint32_t baseLayer, uint32_t level,
			                   VkBuffer dstBuffer, VkDeviceSize bufferOffset)
		{
			QK_PROFILE_FUNCTION();

			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkBufferImageCopy copyRegion{};
			copyRegion.bufferOffset                    = bufferOffset;
			copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel       = level;
			copyRegion.imageSubresource.baseArrayLayer = baseLayer;
			copyRegion.imageSubresource.layerCount     = layerCount;
			copyRegion.imageOffset                     = imageOffset;
			copyRegion.imageExtent                     = imageExtent;
			vkCmdCopyImageToBuffer(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer, 1, &copyRegion);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			VkQueue transferQueue = device->GetTransferQueue();
			vkQueueSubmit(transferQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(transferQueue);
		}

		void GenerateMipmaps(VulkanDevice* device, VkImage image, uint32_t width, uint32_t height, uint32_t levelCount)
		{
			QK_PROFILE_FUNCTION();

			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkImageMemoryBarrier barrier{};
			barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image                           = image;
			barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount     = 1;
			barrier.subresourceRange.levelCount     = 1;

			int32_t mipWidth = width;
			int32_t mipHeight = height;

			for (uint32_t i = 1; i < levelCount; i++)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr, 0, nullptr, 1, &barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0]                 = { 0, 0, 0 };
				blit.srcOffsets[1]                 = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel       = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount     = 1;
				blit.dstOffsets[0]                 = { 0, 0, 0 };
				blit.dstOffsets[1]                 = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel       = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount     = 1;

				vkCmdBlitImage(commandBuffer,
					image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth >>= 1;
				if (mipHeight > 1) mipHeight >>= 2;
			}

			barrier.subresourceRange.baseMipLevel = levelCount - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			// Warning: this queue must have graphics capabilities
			VkQueue transferQueue = device->GetTransferQueue();
			vkQueueSubmit(transferQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(transferQueue);
		}

		void TransitionImageLayout(VulkanDevice* device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t levelCount)
		{
			QK_PROFILE_FUNCTION();

			VkCommandBuffer commandBuffer = device->GetCopyCommandBuffer();
			vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkImageMemoryBarrier barrier{};
			barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout                       = oldLayout;
			barrier.newLayout                       = newLayout;
			barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			barrier.image                           = image;
			barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel   = 0;
			barrier.subresourceRange.levelCount     = levelCount;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount     = 1;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else
			{
				QK_CORE_ASSERT(false, "Unsupported layout transition!");
			}

			vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			// Warning: this queue must have graphics capabilities
			VkQueue transferQueue = device->GetTransferQueue();
			vkQueueSubmit(transferQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(transferQueue);
		}
	}
}

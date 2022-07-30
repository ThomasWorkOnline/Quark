#include "qkpch.h"
#include "VulkanUtils.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>

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

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					return availableFormat;
			}

			QK_CORE_WARN("Swap surface format not found: using default format 0");
			return availableFormats[0];
		}

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return availablePresentMode;
			}

			QK_CORE_WARN("Swap present mode not found: using default FIFO present mode");
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				VkExtent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}

		static uint32_t GetBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

		VkBuffer AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory)
		{
			QK_PROFILE_FUNCTION();

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
			auto vkPhysicalDevice = VulkanContext::GetCurrentDevice()->GetPhysicalDevice();

			VkBuffer buffer;
			vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &buffer);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(vkDevice, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = Utils::GetBufferMemoryType(vkPhysicalDevice, memRequirements.memoryTypeBits, properties);

			vkAllocateMemory(vkDevice, &allocInfo, nullptr, &bufferMemory);
			vkBindBufferMemory(vkDevice, buffer, bufferMemory, 0);

			return buffer;
		}

		// TODO: copy allocator
		void CopyBuffer(VkBuffer dstBuffer, VkBuffer srcBuffer, size_t size)
		{
			QK_PROFILE_FUNCTION();

			auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
			auto commandPool = VulkanContext::GetCurrentDevice()->GetCommandPool();

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;
			
			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(vkDevice, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VkBufferCopy copyRegion{};
			copyRegion.size = size;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			auto graphicsQueue = VulkanContext::GetCurrentDevice()->GetGraphicsQueue();

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
			vkQueueWaitIdle(graphicsQueue);

			vkFreeCommandBuffers(vkDevice, commandPool, 1, &commandBuffer);
		}
	}
}

#include "qkpch.h"
#include "VulkanUtils.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Quark {

	namespace Utils {

		std::vector<const char*> GetRequiredVkExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
			return extensions;
		}

		void EnumerateVkExtensions()
		{
			uint32_t extensionCount = 0;
			vk::Result vkRes = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan extension properties");

			std::vector<vk::ExtensionProperties> extensions(extensionCount);
			vkRes = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan extension properties");

			std::stringstream ss;
			ss << "available extensions:\n";
			for (const auto& extension : extensions)
				ss << '\t' << extension.extensionName << '\n';

			QK_CORE_INFO(ss.str());
		}

		bool CheckVkValidationLayerSupport()
		{
			uint32_t layerCount;
			vk::Result vkRes = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan validation layer properties");

			std::vector<vk::LayerProperties> availableLayers(layerCount);
			vkRes = vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Error enumerating Vulkan validation layer properties");

			for (const char* layerName : g_ValidationLayers)
			{
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers)
				{
					if (std::strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
					return false;
			}

			return true;
		}

		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
					return availableFormat;
			}

			QK_CORE_WARN("Swap surface format not found: using default format 0");
			return availableFormats[0];
		}

		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == vk::PresentModeKHR::eMailbox)
					return availablePresentMode;
			}

			QK_CORE_WARN("Swap present mode not found: using default FIFO present mode");
			return vk::PresentModeKHR::eFifo;
		}

		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				vk::Extent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}

		vk::SurfaceKHR CreateSurfaceForPlatform(vk::Instance instance, GLFWwindow* window)
		{
			VkSurfaceKHR surface;
			glfwCreateWindowSurface(instance, window, nullptr, &surface);
			return surface;
		}

		static uint32_t GetBufferMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
		{
			auto vkPhysicalDevice = VulkanContext::GetCurrentDevice()->GetPhysicalDevice();

			vk::PhysicalDeviceMemoryProperties memProperties = vkPhysicalDevice.getMemoryProperties();
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
					return i;
			}

			QK_CORE_ASSERT(false, "Unable to find suitable memory type for buffer");
			return 0;
		}

		vk::Buffer AllocateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::DeviceMemory& bufferMemory)
		{
			QK_PROFILE_FUNCTION();

			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

			VkBuffer buffer;
			vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &buffer);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(vkDevice, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = Utils::GetBufferMemoryType(memRequirements.memoryTypeBits, properties);

			vkAllocateMemory(vkDevice, &allocInfo, nullptr, reinterpret_cast<VkDeviceMemory*>(&bufferMemory));
			vkBindBufferMemory(vkDevice, buffer, bufferMemory, 0);

			return buffer;
		}

		// TODO: copy allocator
		void CopyBuffer(vk::Buffer dstBuffer, vk::Buffer srcBuffer, size_t size)
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

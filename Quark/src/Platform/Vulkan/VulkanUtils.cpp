#include "qkpch.h"
#include "VulkanUtils.h"
#include "VulkanContext.h"

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

		QueueFamilyIndices FindVkQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
		{
			uint32_t queueFamilyCount = 0;
			device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

			std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
			device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

			QueueFamilyIndices indices;

			uint32_t i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
					indices.GraphicsFamily = i;

				vk::Bool32 presentSupport = device.getSurfaceSupportKHR(i, surface);

				if (presentSupport)
					indices.PresentFamily = i;

				if (indices.IsComplete())
					break;

				i++;
			}

			return indices;
		}

		SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
		{
			SwapChainSupportDetails details;
			details.Capabilities = device.getSurfaceCapabilitiesKHR(surface);

			uint32_t formatCount;
			vk::Result vkRes = device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface formats");

			if (formatCount != 0)
			{
				details.Formats.resize(formatCount);
				vkRes = device.getSurfaceFormatsKHR(surface, &formatCount, details.Formats.data());
				QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface formats");
			}

			uint32_t presentModeCount;
			vkRes = device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface present modes");

			if (presentModeCount != 0)
			{
				details.PresentModes.resize(presentModeCount);
				vkRes = device.getSurfacePresentModesKHR(surface, &presentModeCount, details.PresentModes.data());
				QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not query surface present modes");
			}

			return details;
		}

		bool CheckVkDeviceExtensionSupport(vk::PhysicalDevice device)
		{
			uint32_t extensionCount;
			vk::Result vkRes = device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate device extension properties");

			std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
			vkRes = device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
			QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate device extension properties");

			for (const char* extensionName : g_DeviceExtensions)
			{
				bool extensionFound = false;
				for (const auto& extension : availableExtensions)
				{
					if (std::strcmp(extensionName, extension.extensionName) == 0)
					{
						extensionFound = true;
						break;
					}
				}

				if (!extensionFound)
					return false;
			}

			return true;
		}

		bool IsVkDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const QueueFamilyIndices& indices)
		{
			bool extensionsSupported = CheckVkDeviceExtensionSupport(device);

			bool swapChainAdequate = false;
			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
				swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
			}

			return indices.IsComplete() && swapChainAdequate;
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

		uint32_t GetBufferMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
		{
			auto vkPhysicalDevice = VulkanContext::GetCurrentDevice().GetPhysicalDevice();
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

			vk::BufferCreateInfo bufferInfo;
			bufferInfo.setSize(size);
			bufferInfo.setUsage(usage);
			bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			vk::Buffer buffer = vkDevice.createBuffer(bufferInfo);

			vk::MemoryRequirements memRequirements = vkDevice.getBufferMemoryRequirements(buffer);
			uint32_t memoryTypeIndex = Utils::GetBufferMemoryType(memRequirements.memoryTypeBits, properties);

			vk::MemoryAllocateInfo allocInfo;
			allocInfo.setAllocationSize(memRequirements.size);
			allocInfo.setMemoryTypeIndex(memoryTypeIndex);

			bufferMemory = vkDevice.allocateMemory(allocInfo);
			vkDevice.bindBufferMemory(buffer, bufferMemory, 0);

			return buffer;
		}

		// TODO: copy allocator
		void CopyBuffer(vk::Buffer dstBuffer, vk::Buffer srcBuffer, size_t size)
		{
			QK_PROFILE_FUNCTION();

			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			auto commandPool = VulkanContext::GetCurrentDevice().GetCommandPool();

			vk::CommandBufferAllocateInfo allocInfo;
			allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
			allocInfo.setCommandPool(commandPool);
			allocInfo.setCommandBufferCount(1);

			vk::CommandBuffer commandBuffer = vkDevice.allocateCommandBuffers(allocInfo)[0];

			vk::CommandBufferBeginInfo beginInfo;
			beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			vk::BufferCopy copyRegion;
			copyRegion.setSize(size);

			commandBuffer.begin(beginInfo);
			commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
			commandBuffer.end();

			vk::SubmitInfo submitInfo;
			submitInfo.setCommandBufferCount(1);
			submitInfo.setPCommandBuffers(&commandBuffer);

			auto graphicsQueue = VulkanContext::GetCurrentDevice().GetGraphicsQueue();

			graphicsQueue.submit(submitInfo);
			graphicsQueue.waitIdle();

			vkDevice.freeCommandBuffers(commandPool, 1, &commandBuffer);
		}
	}
}

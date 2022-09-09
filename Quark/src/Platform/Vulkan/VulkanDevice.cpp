#include "qkpch.h"
#include "VulkanDevice.h"

#include "VulkanUtils.h"
#include "VulkanContext.h"

#include <set>

namespace Quark {

	static const char* s_DeviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	namespace Utils {

		static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			SwapChainSupportDetails details;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

			uint32_t formatCount;
			VkResult vkRes = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not query surface formats");

			if (formatCount != 0)
			{
				details.Formats.resize(formatCount);
				VkResult vkRes = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
				QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not query surface formats");
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not query surface present modes");

			if (presentModeCount != 0)
			{
				details.PresentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
				QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not query surface present modes");
			}

			return details;
		}

		static QueueFamilyIndices FindVkQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			QueueFamilyIndices indices;

			uint32_t i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.GraphicsFamily = i;

				VkBool32 presentSupport;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport)
					indices.PresentFamily = i;

				if (indices.IsComplete())
					break;

				i++;
			}

			return indices;
		}

		static bool CheckVkDeviceExtensionSupport(VkPhysicalDevice device)
		{
			uint32_t extensionCount;
			VkResult vkRes = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not enumerate device extension properties");

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkRes = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not enumerate device extension properties");

			for (const char* extensionName : s_DeviceExtensions)
			{
				bool extensionFound = false;
				for (auto& extension : availableExtensions)
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

		static bool IsVkDeviceSuitable(VkPhysicalDevice device, const SwapChainSupportDetails& supportDetails, const QueueFamilyIndices& indices)
		{
			bool extensionsSupported = CheckVkDeviceExtensionSupport(device);
			bool swapChainAdequate = !supportDetails.Formats.empty() && !supportDetails.PresentModes.empty();

			VkPhysicalDeviceFeatures supportedFeatures;
			vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

			return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
		}
	}

	VulkanDevice::VulkanDevice(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, QueueFamilyIndices queueFamilyIndices, SwapChainSupportDetails supportDetails)
		: m_PhysicalDevice(vkPhysicalDevice)
		, m_QueueFamilyIndices(std::move(queueFamilyIndices))
		, m_SupportDetails(std::move(supportDetails))
	{
		QK_PROFILE_FUNCTION();

		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = sizeof_array(s_DeviceExtensions);
		createInfo.ppEnabledExtensionNames = s_DeviceExtensions;

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		createInfo.enabledLayerCount = sizeof_array(g_ValidationLayers);
		createInfo.ppEnabledLayerNames = g_ValidationLayers;
#endif
		vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &m_Device);

		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = VulkanContext::Get()->GetInstance();
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);

		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();

			vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
			vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.PresentFamily.value(), 0, &m_PresentQueue);

			vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = m_CommandPool;
			allocInfo.commandBufferCount = 1;

			vkAllocateCommandBuffers(m_Device, &allocInfo, &m_CopyCommandBuffer);
		}
	}

	VulkanDevice::~VulkanDevice()
	{
		QK_PROFILE_FUNCTION();

		vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CopyCommandBuffer);
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		vmaDestroyAllocator(m_Allocator);
		vkDestroyDevice(m_Device, nullptr);
	}

	void VulkanDevice::WaitUntilIdle() const
	{
		vkDeviceWaitIdle(m_Device);
	}

	Scope<VulkanDevice> VulkanDevice::CreateDefaultForSurface(VkInstance vkInstance, VkSurfaceKHR vkSurface)
	{
		QK_PROFILE_FUNCTION();

		uint32_t deviceCount = 0;
		VkResult vkRes = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not enumerate physical devices");
		QK_CORE_ASSERT(deviceCount, "No physical graphics devices found");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkRes = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not enumerate physical devices");

		VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
		QueueFamilyIndices queueFamilyIndices;
		SwapChainSupportDetails supportDetails;

		for (const auto& device : devices)
		{
			auto queueFamilies = Utils::FindVkQueueFamilies(device, vkSurface);
			auto support = Utils::QuerySwapChainSupport(device, vkSurface);
			if (Utils::IsVkDeviceSuitable(device, support, queueFamilies))
			{
				vkPhysicalDevice = device;
				queueFamilyIndices = std::move(queueFamilies);
				supportDetails = std::move(support);
				break;
			}
		}

		QK_CORE_ASSERT(vkPhysicalDevice, "Failed to find a suitable graphics device");

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			*queueFamilyIndices.GraphicsFamily,
			*queueFamilyIndices.PresentFamily
		};

		float queuePriorities = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriorities;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		return CreateScope<VulkanDevice>(vkPhysicalDevice, queueCreateInfos, std::move(queueFamilyIndices), std::move(supportDetails));
	}
}

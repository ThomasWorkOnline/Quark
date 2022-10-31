#include "qkpch.h"
#include "VulkanDevice.h"

#include "VulkanUtils.h"
#include "VulkanContext.h"

#include <set>

namespace Quark {

	static const char* s_DeviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE1_EXTENSION_NAME
	};

	namespace Utils {

		static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			SwapChainSupportDetails details;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

			{
				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

				details.Formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
			}

			{
				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

				details.PresentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
			}

			return details;
		}

		static QueueFamilyIndices FindVkQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			AutoRelease<VkQueueFamilyProperties> queueFamilies = StackAlloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

			QueueFamilyIndices indices;
			for (uint32_t i = 0; i < queueFamilyCount; i++)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.GraphicsFamily = i;

				if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
					indices.TransferFamily = i;

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
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

			AutoRelease<VkExtensionProperties> availableExtensions = StackAlloc(extensionCount * sizeof(VkExtensionProperties));
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions);

			for (const char* extensionName : s_DeviceExtensions)
			{
				bool extensionFound = false;
				for (uint32_t i = 0; i < extensionCount; i++)
				{
					if (std::strcmp(extensionName, availableExtensions[i].extensionName) == 0)
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

	VulkanDevice::VulkanDevice(VkPhysicalDevice vkPhysicalDevice, const QueueFamilyIndices& queueFamilyIndices, SwapChainSupportDetails supportDetails)
		: m_PhysicalDevice(vkPhysicalDevice)
		, m_QueueFamilyIndices(queueFamilyIndices)
		, m_SupportDetails(std::move(supportDetails))
	{
		QK_PROFILE_FUNCTION();

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.shaderFloat64     = VK_TRUE;

		std::set<uint32_t> uniqueQueueFamilies = {
			*m_QueueFamilyIndices.GraphicsFamily,
			*m_QueueFamilyIndices.PresentFamily,
			*m_QueueFamilyIndices.TransferFamily
		};

		AutoRelease<VkDeviceQueueCreateInfo> queueCreateInfos = StackAlloc(uniqueQueueFamilies.size() * sizeof(VkDeviceQueueCreateInfo));
		VkDeviceQueueCreateInfo* queueCreateInfoPtr = queueCreateInfos;

		float queuePriorities = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			*queueCreateInfoPtr = {};
			queueCreateInfoPtr->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfoPtr->queueFamilyIndex = queueFamily;
			queueCreateInfoPtr->queueCount = 1;
			queueCreateInfoPtr->pQueuePriorities = &queuePriorities;
			queueCreateInfoPtr++;
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
		createInfo.pQueueCreateInfos = queueCreateInfos;
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = sizeof_array(s_DeviceExtensions);
		createInfo.ppEnabledExtensionNames = s_DeviceExtensions;

#if QK_ENABLE_VULKAN_VALIDATION_LAYERS
		createInfo.enabledLayerCount = sizeof_array(g_ValidationLayers);
		createInfo.ppEnabledLayerNames = g_ValidationLayers;
#endif
		vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &m_Device);
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);

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
			vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.TransferFamily.value(), 0, &m_TransferQueue);

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

	const char* VulkanDevice::GetName() const
	{
		return m_Properties.deviceName;
	}

	void VulkanDevice::WaitUntilIdle() const
	{
		vkDeviceWaitIdle(m_Device);
	}

	Scope<VulkanDevice> VulkanDevice::CreateDefaultForSurface(VkInstance vkInstance, VkSurfaceKHR vkSurface)
	{
		QK_PROFILE_FUNCTION();

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

		AutoRelease<VkPhysicalDevice> devices = StackAlloc(deviceCount * sizeof(VkPhysicalDevice));
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices);

		for (uint32_t i = 0; i < deviceCount; i++)
		{
			auto queueFamilies = Utils::FindVkQueueFamilies(devices[i], vkSurface);
			auto support = Utils::QuerySwapChainSupport(devices[i], vkSurface);
			if (Utils::IsVkDeviceSuitable(devices[i], support, queueFamilies))
			{
				return CreateScope<VulkanDevice>(devices[i], queueFamilies, std::move(support));
			}
		}

		QK_CORE_ASSERT(false, "Failed to find a suitable graphics device");
		return nullptr;
	}
}

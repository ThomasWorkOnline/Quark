#include "qkpch.h"
#include "VulkanDevice.h"

#include "VulkanUtils.h"

#include <set>

namespace Quark {

	namespace Utils {

		static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
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

		static QueueFamilyIndices FindVkQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
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

		static bool CheckVkDeviceExtensionSupport(vk::PhysicalDevice device)
		{
			uint32_t extensionCount;
			VkResult vkRes = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not enumerate device extension properties");

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkRes = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not enumerate device extension properties");

			for (const char* extensionName : g_DeviceExtensions)
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

		static bool IsVkDeviceSuitable(vk::PhysicalDevice device, const SwapChainSupportDetails& supportDetails, const QueueFamilyIndices& indices)
		{
			bool extensionsSupported = CheckVkDeviceExtensionSupport(device);

			bool swapChainAdequate = false;
			if (extensionsSupported)
			{
				swapChainAdequate = !supportDetails.Formats.empty() && !supportDetails.PresentModes.empty();
			}

			return indices.IsComplete() && swapChainAdequate;
		}
	}

	VulkanDevice::VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, QueueFamilyIndices queueFamilyIndices, SwapChainSupportDetails supportDetails)
		: m_Device(vkDevice),
		m_PhysicalDevice(vkPhysicalDevice),
		m_QueueFamilyIndices(std::move(queueFamilyIndices)),
		m_SupportDetails(std::move(supportDetails))
	{
		QK_PROFILE_FUNCTION();

		{
			vk::CommandPoolCreateInfo poolInfo;
			poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
			poolInfo.setQueueFamilyIndex(*m_QueueFamilyIndices.GraphicsFamily);

			m_GraphicsQueue = m_Device.getQueue(m_QueueFamilyIndices.GraphicsFamily.value(), 0);
			m_PresentQueue = m_Device.getQueue(m_QueueFamilyIndices.PresentFamily.value(), 0);

			m_CommandPool = m_Device.createCommandPool(poolInfo);
		}
	}

	VulkanDevice::~VulkanDevice()
	{
		QK_PROFILE_FUNCTION();

		m_Device.destroyCommandPool(m_CommandPool);
		m_Device.destroy();
	}

	Scope<VulkanDevice> VulkanDevice::CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface)
	{
		QK_PROFILE_FUNCTION();

		vk::PhysicalDevice vkPhysicalDevice;
		QueueFamilyIndices queueFamilyIndices;
		SwapChainSupportDetails supportDetails;

		uint32_t deviceCount = 0;
		vk::Result vkRes = vkInstance.enumeratePhysicalDevices(&deviceCount, nullptr);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate physical devices");
		QK_CORE_ASSERT(deviceCount, "No physical graphics devices found");

		std::vector<vk::PhysicalDevice> devices(deviceCount);
		vkRes = vkInstance.enumeratePhysicalDevices(&deviceCount, devices.data());
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not enumerate physical devices");

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

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			*queueFamilyIndices.GraphicsFamily,
			*queueFamilyIndices.PresentFamily
		};

		float queuePriorities = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setQueueFamilyIndex(queueFamily);
			queueCreateInfo.setQueueCount(1);
			queueCreateInfo.setPQueuePriorities(&queuePriorities);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures;

		vk::DeviceCreateInfo createInfo;
		createInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
		createInfo.setPQueueCreateInfos(queueCreateInfos.data());
		createInfo.setPEnabledFeatures(&deviceFeatures);

		createInfo.setEnabledExtensionCount(static_cast<uint32_t>(g_DeviceExtensions.size()));
		createInfo.setPpEnabledExtensionNames(g_DeviceExtensions.data());

#ifdef QK_ENABLE_VULKAN_VALIDATION_LAYERS
		createInfo.setEnabledLayerCount(static_cast<uint32_t>(g_ValidationLayers.size()));
		createInfo.setPpEnabledLayerNames(g_ValidationLayers.data());
#endif
		auto vkDevice = vkPhysicalDevice.createDevice(createInfo, nullptr);

		return CreateScope<VulkanDevice>(vkDevice, vkPhysicalDevice, std::move(queueFamilyIndices), std::move(supportDetails));
	}
}

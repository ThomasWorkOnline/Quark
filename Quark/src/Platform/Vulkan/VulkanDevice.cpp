#include "qkpch.h"
#include "VulkanDevice.h"

#include <set>

namespace Quark {

	VulkanDevice::VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, const Utils::QueueFamilyIndices& queueFamilyIndices)
		: m_Device(vkDevice),
		m_PhysicalDevice(vkPhysicalDevice),
		m_QueueFamilyIndices(queueFamilyIndices)
	{
		QK_PROFILE_FUNCTION();

		{
			vk::CommandPoolCreateInfo poolInfo;
			poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
			poolInfo.setQueueFamilyIndex(*m_QueueFamilyIndices.GraphicsFamily);

			m_GraphicsQueue = m_Device.getQueue(m_QueueFamilyIndices.GraphicsFamily.value(), 0);
			m_PresentQueue = m_Device.getQueue(m_QueueFamilyIndices.PresentFamily.value(), 0);

			m_CommandPool = m_Device.createCommandPool(poolInfo);

			vk::CommandBufferAllocateInfo allocInfo;
			allocInfo.setCommandPool(m_CommandPool);
			allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
			allocInfo.setCommandBufferCount(g_FramesInFlight);

			m_CommandBuffers.resize(g_FramesInFlight);
			m_CommandBuffers = m_Device.allocateCommandBuffers(allocInfo);
		}
	}

	VulkanDevice::~VulkanDevice()
	{
		QK_PROFILE_FUNCTION();

		m_Device.waitIdle();
		m_Device.destroyCommandPool(m_CommandPool);
		m_Device.destroy();
	}

	Scope<VulkanDevice> VulkanDevice::CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface)
	{
		QK_PROFILE_FUNCTION();

		vk::PhysicalDevice vkPhysicalDevice;
		Utils::QueueFamilyIndices queueFamilyIndices;

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
			if (Utils::IsVkDeviceSuitable(device, vkSurface, queueFamilies))
			{
				vkPhysicalDevice = device;
				queueFamilyIndices = queueFamilies;
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

		return CreateScope<VulkanDevice>(vkDevice, vkPhysicalDevice, queueFamilyIndices);
	}
}

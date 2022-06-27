#pragma once

#include "VulkanUtils.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanDevice
	{
	public:
		VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, const Utils::QueueFamilyIndices& queueFamilyIndices);
		~VulkanDevice();

		vk::Device GetVkHandle() const { return m_VkDevice; }

		vk::CommandPool GetCommandPool() const { return m_VkCommandPool; }
		vk::PhysicalDevice GetPhysicalDevice() const { return m_VkPhysicalDevice; }

		vk::Queue GetGraphicsQueue() const { return m_VkGraphicsQueue; }
		vk::Queue GetPresentQueue() const { return m_VkPresentQueue; }

		const Utils::QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		static Scope<VulkanDevice> CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface);

	private:
		vk::Device m_VkDevice;
		vk::PhysicalDevice m_VkPhysicalDevice;
		vk::CommandPool m_VkCommandPool;
		vk::Queue m_VkGraphicsQueue;
		vk::Queue m_VkPresentQueue;

		Utils::QueueFamilyIndices m_QueueFamilyIndices;
	};
}

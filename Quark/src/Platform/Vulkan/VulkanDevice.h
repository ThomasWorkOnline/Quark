#pragma once

#include "VulkanCore.h"

namespace Quark {

	class VulkanDevice
	{
	public:
		VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, const Utils::QueueFamilyIndices& queueFamilyIndices);
		~VulkanDevice();

		vk::Device GetVkHandle() const { return m_VkDevice; }
		vk::CommandBuffer GetCommandBuffer() const { return m_VkCommandBuffer; }
		vk::PhysicalDevice GetPhysicalDevice() const { return m_VkPhysicalDevice; }

		const Utils::QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		static Scope<VulkanDevice> CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface);

	private:
		vk::Device m_VkDevice;
		vk::PhysicalDevice m_VkPhysicalDevice;

		vk::CommandPool m_VkCommandPool;
		vk::CommandBuffer m_VkCommandBuffer;

		Utils::QueueFamilyIndices m_QueueFamilyIndices;
	};
}

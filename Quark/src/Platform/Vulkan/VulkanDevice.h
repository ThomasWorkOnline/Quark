#pragma once

#include "VulkanUtils.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanDevice
	{
	public:
		VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, const Utils::QueueFamilyIndices& queueFamilyIndices);
		~VulkanDevice();

		vk::CommandPool GetCommandPool() const { return m_VkCommandPool; }
		vk::PhysicalDevice GetPhysicalDevice() const { return m_VkPhysicalDevice; }

		vk::Queue GetGraphicsQueue() const { return m_VkGraphicsQueue; }
		vk::Queue GetPresentQueue() const { return m_VkPresentQueue; }

		vk::CommandBuffer GetCommandBuffer() const { return m_VkActiveCommandBuffer; }
		vk::CommandBuffer SwitchCommandBuffer(uint32_t index) { m_VkActiveCommandBuffer = m_VkCommandBuffers[index]; return m_VkActiveCommandBuffer; }

		vk::Device GetVkHandle() const { return m_VkDevice; }

		const Utils::QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		static Scope<VulkanDevice> CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface);

	private:
		vk::Device m_VkDevice;
		vk::PhysicalDevice m_VkPhysicalDevice;

		vk::Queue m_VkGraphicsQueue;
		vk::Queue m_VkPresentQueue;

		vk::CommandPool m_VkCommandPool;
		vk::CommandBuffer m_VkActiveCommandBuffer;
		std::vector<vk::CommandBuffer> m_VkCommandBuffers;

		Utils::QueueFamilyIndices m_QueueFamilyIndices;
	};
}

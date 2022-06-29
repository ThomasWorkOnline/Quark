#pragma once

#include "VulkanUtils.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanDevice
	{
	public:
		VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, const Utils::QueueFamilyIndices& queueFamilyIndices);
		~VulkanDevice();

		vk::CommandPool GetCommandPool() const { return m_CommandPool; }
		vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

		vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }
		vk::Queue GetPresentQueue() const { return m_PresentQueue; }

		vk::CommandBuffer GetCommandBuffer() const { return m_ActiveCommandBuffer; }
		vk::CommandBuffer SwitchCommandBuffer(uint32_t index) { m_ActiveCommandBuffer = m_CommandBuffers[index]; return m_ActiveCommandBuffer; }

		vk::Device GetVkHandle() const { return m_Device; }

		const Utils::QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		static Scope<VulkanDevice> CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface);

	private:
		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;

		vk::Queue m_GraphicsQueue;
		vk::Queue m_PresentQueue;

		vk::CommandPool m_CommandPool;
		vk::CommandBuffer m_ActiveCommandBuffer;
		std::vector<vk::CommandBuffer> m_CommandBuffers;

		Utils::QueueFamilyIndices m_QueueFamilyIndices;
	};
}

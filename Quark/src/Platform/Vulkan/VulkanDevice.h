#pragma once

#include "Quark/Core/Core.h"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace Quark {

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR        Capabilities;
		std::vector<vk::SurfaceFormatKHR> Formats;
		std::vector<vk::PresentModeKHR>   PresentModes;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(vk::Device vkDevice, vk::PhysicalDevice vkPhysicalDevice, QueueFamilyIndices queueFamilyIndices, SwapChainSupportDetails supportDetails);
		~VulkanDevice();

		void WaitIdle() const { m_Device.waitIdle(); }

		vk::CommandPool GetCommandPool() const { return m_CommandPool; }
		vk::PhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

		vk::Queue GetGraphicsQueue() const { return m_GraphicsQueue; }
		vk::Queue GetPresentQueue() const { return m_PresentQueue; }

		vk::Device GetVkHandle() const { return m_Device; }

		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		const SwapChainSupportDetails& GetSupportDetails() const { return m_SupportDetails; }

		static Scope<VulkanDevice> CreateDefaultForSurface(vk::Instance vkInstance, vk::SurfaceKHR vkSurface);

	private:
		vk::Device m_Device;
		vk::PhysicalDevice m_PhysicalDevice;

		vk::Queue m_GraphicsQueue;
		vk::Queue m_PresentQueue;
		vk::CommandPool m_CommandPool;

		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SupportDetails;
	};
}

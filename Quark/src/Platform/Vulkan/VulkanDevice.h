#pragma once

#include "Quark/Core/Core.h"

#include <vulkan/vulkan.hpp>
#include <optional>

namespace Quark {

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR>   PresentModes;
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
		VulkanDevice(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos, QueueFamilyIndices queueFamilyIndices, SwapChainSupportDetails supportDetails);
		~VulkanDevice();

		void WaitIdle() const;

		VkCommandPool GetCommandPool() const { return m_CommandPool; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() const { return m_PresentQueue; }

		VkDevice GetVkHandle() const { return m_Device; }

		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		const SwapChainSupportDetails& GetSupportDetails() const { return m_SupportDetails; }

		static Scope<VulkanDevice> CreateDefaultForSurface(VkInstance vkInstance, VkSurfaceKHR vkSurface);

	private:
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkCommandPool m_CommandPool;

		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SupportDetails;
	};
}

#pragma once

#include "Quark/Core/Core.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

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

		const char* GetName() const;
		void WaitUntilIdle() const;

		VkDevice GetVkHandle() const { return m_Device; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

		VmaAllocator GetAllocator() const { return m_Allocator; }

		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() const { return m_PresentQueue; }

		VkCommandPool GetCommandPool() const { return m_CommandPool; }
		VkCommandBuffer GetCopyCommandBuffer() const { return m_CopyCommandBuffer; }

		const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_Properties; }

		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		const SwapChainSupportDetails& GetSupportDetails() const { return m_SupportDetails; }

		static Scope<VulkanDevice> CreateDefaultForSurface(VkInstance vkInstance, VkSurfaceKHR vkSurface);

		// Non-Copyable
		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator=(const VulkanDevice&) = delete;

		// Non-Movable
		VulkanDevice(VulkanDevice&&) = delete;
		VulkanDevice& operator=(VulkanDevice&&) = delete;

	private:
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;
		VkPhysicalDeviceProperties m_Properties;

		VmaAllocator m_Allocator;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CopyCommandBuffer;

		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SupportDetails;
	};
}

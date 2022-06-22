#pragma once

#include "VulkanCore.h"

namespace Quark {

	struct VulkanSwapChainSpecification
	{
		vk::SurfaceFormatKHR       SurfaceFormat{};
		vk::PresentModeKHR         PresentMode{};
		vk::Extent2D               Extent;

		Utils::QueueFamilyIndices  FamilyIndices;
		uint32_t                   ImageCount = 0;
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(vk::Device device, vk::SurfaceKHR surface, const VulkanSwapChainSpecification& spec);
		~VulkanSwapChain();

		void Present();
		uint32_t AcquireNextImageIndex();

		vk::ImageView GetImageView(uint32_t i) const { return m_VkSwapChainImageViews[i]; }
		vk::Queue GetPresentQueue() const { return m_VkPresentQueue; }

		vk::Semaphore GetRenderFinishedSemaphore() const { return m_VkRenderFinishedSemaphore; }
		vk::Semaphore GetImageAvailableSemaphore() const { return m_VkImageAvailableSemaphore; }

		const VulkanSwapChainSpecification& GetSpecification() const { return m_Spec; }

	private:
		vk::Device m_VkDevice;
		vk::SurfaceKHR m_VkSurface;
		vk::SwapchainKHR m_VkSwapChain;
		vk::Queue m_VkPresentQueue;

		vk::Semaphore m_VkRenderFinishedSemaphore;
		vk::Semaphore m_VkImageAvailableSemaphore;

		std::vector<vk::Image> m_VkSwapChainImages;
		std::vector<vk::ImageView> m_VkSwapChainImageViews;
		uint32_t m_NextFrameIndex = 0;

		VulkanSwapChainSpecification m_Spec;
	};
}

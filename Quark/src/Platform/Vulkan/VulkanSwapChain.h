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
		VulkanSwapChain(vk::SurfaceKHR surface, const VulkanSwapChainSpecification& spec);
		~VulkanSwapChain();

		void Present();
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		uint32_t AcquireNextImageIndex(uint32_t frameIndex);

		vk::ImageView GetImageView(uint32_t i) const { return m_VkSwapChainImageViews[i]; }
		vk::Queue GetPresentQueue() const { return m_VkPresentQueue; }

		vk::Semaphore GetRenderFinishedSemaphore(uint32_t i) const { return m_VkRenderFinishedSemaphores[i]; }
		vk::Semaphore GetImageAvailableSemaphore(uint32_t i) const { return m_VkImageAvailableSemaphores[i]; }

		const VulkanSwapChainSpecification& GetSpecification() const { return m_Spec; }

	private:
		void Invalidate(uint32_t viewportWidth, uint32_t viewportHeight);

	private:
		vk::Device m_VkDevice;
		vk::SurfaceKHR m_VkSurface;
		vk::SwapchainKHR m_VkSwapChain;
		vk::Queue m_VkPresentQueue;

		std::vector<vk::Image> m_VkSwapChainImages;
		std::vector<vk::ImageView> m_VkSwapChainImageViews;

		std::vector<vk::Semaphore> m_VkRenderFinishedSemaphores;
		std::vector<vk::Semaphore> m_VkImageAvailableSemaphores;

		uint32_t m_CurrentFrameIndex = 0;
		uint32_t m_ImageIndex = 0;
		VulkanSwapChainSpecification m_Spec;
	};
}

#pragma once

#include "VulkanUtils.h"
#include <vulkan/vulkan.hpp>

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

		void Present(vk::Queue presentQueue);
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		uint32_t AcquireNextImageIndex(vk::Semaphore imageAvailableSemaphore, uint32_t frameIndex);

		vk::ImageView GetImageView(uint32_t i) const { return m_VkSwapChainImageViews[i]; }
		vk::Semaphore GetRenderFinishedSemaphore(uint32_t i) const { return m_VkRenderFinishedSemaphores[i]; }

		const VulkanSwapChainSpecification& GetSpecification() const { return m_Spec; }

	private:
		void Invalidate(uint32_t viewportWidth, uint32_t viewportHeight);

	private:
		vk::SurfaceKHR m_VkSurface;
		vk::SwapchainKHR m_VkSwapChain;

		std::vector<vk::Image> m_VkSwapChainImages;
		std::vector<vk::ImageView> m_VkSwapChainImageViews;
		std::vector<vk::Semaphore> m_VkRenderFinishedSemaphores;

		uint32_t m_CurrentFrameIndex = 0;
		uint32_t m_ImageIndex = 0;
		VulkanSwapChainSpecification m_Spec;
	};
}

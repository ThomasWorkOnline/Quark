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

		uint32_t AcquireNextImageIndex(vk::Semaphore imageAvailableSemaphore);

		vk::ImageView GetImageView(uint32_t i) const { return m_SwapChainImageViews[i]; }
		vk::Semaphore GetRenderFinishedSemaphore(uint32_t i) const { return m_RenderFinishedSemaphores[i]; }

		const VulkanSwapChainSpecification& GetSpecification() const { return m_Spec; }

	private:
		void Invalidate(uint32_t viewportWidth, uint32_t viewportHeight);

	private:
		vk::SurfaceKHR m_Surface;
		vk::SwapchainKHR m_SwapChain;

		std::vector<vk::Image> m_SwapChainImages;
		std::vector<vk::ImageView> m_SwapChainImageViews;
		std::vector<vk::Semaphore> m_RenderFinishedSemaphores;

		uint32_t m_CurrentFrameIndex = std::numeric_limits<uint32_t>::max();
		uint32_t m_ImageIndex = 0;
		VulkanSwapChainSpecification m_Spec;
	};
}

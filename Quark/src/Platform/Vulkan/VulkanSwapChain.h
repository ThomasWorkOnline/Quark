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

		void SwapBuffers();
		void Present(vk::Queue queue);

		uint32_t GetNextImageIndex() const { return m_NextFrameIndex; }
		vk::ImageView GetImageView(uint32_t i) const { return m_VkSwapChainImageViews[i]; }

		vk::Semaphore GetImageAvailableSemaphore() const { return m_VkImageAvailableSemaphore; }
		vk::Semaphore GetRenderFinishedSemaphore() const { return m_VkRenderFinishedSemaphore; }
		vk::Fence GetInFlightFence() const { return m_VkInFlightFence; }

		const VulkanSwapChainSpecification& GetSpecification() const { return m_Spec; }

	private:
		vk::Device m_VkDevice;
		vk::SurfaceKHR m_VkSurface;

		vk::SwapchainKHR m_VkSwapChain;
		std::vector<vk::Image> m_VkSwapChainImages;
		std::vector<vk::ImageView> m_VkSwapChainImageViews;
		uint32_t m_NextFrameIndex = 0;

		vk::Semaphore m_VkImageAvailableSemaphore;
		vk::Semaphore m_VkRenderFinishedSemaphore;
		vk::Fence m_VkInFlightFence;

		VulkanSwapChainSpecification m_Spec;
	};
}

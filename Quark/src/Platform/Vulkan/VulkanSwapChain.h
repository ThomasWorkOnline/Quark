#pragma once

#include "Quark/Core/Core.h"

#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"

#include <vulkan/vulkan.h>

namespace Quark {

	struct VulkanSwapChainSpecification
	{
		uint32_t           MinImageCount = 0;
		VkExtent2D         Extent{};
		VkSurfaceFormatKHR SurfaceFormat{};
		VkPresentModeKHR   PresentMode{};
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, const VulkanSwapChainSpecification& spec);
		~VulkanSwapChain();

		VkResult AcquireNextImage(VkSemaphore imageAvailableSemaphore);
		void Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore);
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		VulkanFramebufferAttachment* GetColorAttachment(uint32_t imageIndex) { return &m_ColorAttachments[imageIndex]; }

		uint32_t GetWidth() const { return m_Spec.Extent.width; }
		uint32_t GetHeight() const { return m_Spec.Extent.width; }
		uint32_t GetImageCount() const { return (uint32_t)m_SwapChainImages.size(); }
		uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }

		const VulkanSwapChainSpecification& GetSpecification() const { return m_Spec; }

		// Non-Copyable
		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain = nullptr;

		VulkanSwapChainSpecification m_Spec;
		uint32_t m_ImageIndex = 0;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VulkanFramebufferAttachment> m_ColorAttachments;
	};
}

#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/SwapChain.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

namespace Quark {

	struct VulkanSwapChainSpecification
	{
		uint32_t           MinImageCount = 0;
		VkExtent2D         Extent{};
		VkSurfaceKHR       Surface;
		VkSurfaceFormatKHR SurfaceFormat{};
		VkPresentModeKHR   PresentMode{};
		const RenderPass*  RenderPass = nullptr;
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, const SwapChainSpecification& spec);
		~VulkanSwapChain();

		VkResult AcquireNextImage(VkSemaphore imageAvailableSemaphore);
		void Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore);
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		Framebuffer* GetFramebuffer() const;

		uint32_t GetWidth() const { return m_Spec.Extent.Width; }
		uint32_t GetHeight() const { return m_Spec.Extent.Height; }
		uint32_t GetImageCount() const { return (uint32_t)m_SwapChainImages.size(); }
		uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }

		const SwapChainSpecification& GetSpecification() const { return m_Spec; }

		// Non-Copyable
		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;
		VkSwapchainKHR m_SwapChain = nullptr;
		VkSurfaceKHR m_Surface;

		SwapChainSpecification m_Spec;
		uint32_t m_ImageIndex = 0;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<Scope<VulkanFramebuffer>> m_Framebuffers;
	};
}

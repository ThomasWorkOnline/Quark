#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/SwapChain.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"

namespace Quark {

	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, const SwapChainSpecification& spec);
		virtual ~VulkanSwapChain() final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual uint32_t GetBufferCount() const final override;
		virtual uint32_t GetCurrentImageIndex() const final override { return m_ImageIndex; }

		virtual Ref<FramebufferAttachment> GetColorAttachment(uint32_t index) const final override;
		virtual Ref<FramebufferAttachment> GetDepthAttachment(uint32_t index) const final override;
		virtual Ref<FramebufferAttachment> GetResolveAttachment(uint32_t index) const final override;

		void Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore);
		void SetPresentMode(SwapPresentMode presentMode);

		VkResult AcquireNextImage(VkSemaphore imageAvailableSemaphore);

		// Non-Copyable
		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device = nullptr;
		VkSwapchainKHR m_SwapChain = nullptr;
		VkSurfaceKHR m_Surface = nullptr;

		uint32_t m_ImageIndex = 0;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<Ref<VulkanFramebufferAttachment>> m_ColorAttachments;
		std::vector<Ref<VulkanFramebufferAttachment>> m_DepthAttachments;
		std::vector<Ref<VulkanFramebufferAttachment>> m_ResolveAttachments;
	};
}

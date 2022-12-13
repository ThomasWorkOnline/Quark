#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/SwapChain.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain() = default;
		VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, const SwapChainSpecification& spec);
		virtual ~VulkanSwapChain() final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual uint32_t GetBufferCount() const final override { return (uint32_t)m_SwapChainImages.size(); }
		virtual uint32_t GetCurrentImageIndex() const final override { return m_ImageIndex; }

		void     Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore);
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
	};
}

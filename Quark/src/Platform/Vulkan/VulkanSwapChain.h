#pragma once

#include "Quark/Renderer/SwapChain.h"
#include "Quark/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain final : public SwapChain
	{
	public:
		VulkanSwapChain(GLFWwindow* window, vk::SurfaceKHR surface, const SwapChainSpecification& spec);
		~VulkanSwapChain();

		virtual uint32_t GetWidth() const override { return m_Format.Extent.width; }
		virtual uint32_t GetHeight() const override { return m_Format.Extent.height; }
		virtual uint32_t GetImageCount() const override { return m_Format.ImageCount; }

		void Present(vk::Queue presentQueue);
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		uint32_t AcquireNextImageIndex(vk::Semaphore imageAvailableSemaphore);

		vk::ImageView GetImageView(uint32_t i) const { return m_SwapChainImageViews[i]; }
		vk::Fence     GetFence() const { return m_InFlightFences[m_CurrentFrameIndex]; }
		vk::Semaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentFrameIndex]; }

	private:
		void Invalidate();

	private:
		struct Format
		{
			uint32_t             ImageCount = 0;
			vk::Extent2D         Extent;
			vk::SurfaceFormatKHR ActualSurfaceFormat;
			vk::PresentModeKHR   ActualPresentMode{};
		};

		GLFWwindow* m_WindowHandle;
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		std::vector<VkFence> m_InFlightFences;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;

		uint32_t m_ImageIndex = 0;
		uint32_t m_CurrentFrameIndex = std::numeric_limits<uint32_t>::max();

		Format m_Format;

		// TODO: remove
		friend class VulkanRenderPass;
	};
}

#pragma once

#include "Quark/Renderer/SwapChain.h"
#include "Quark/Renderer/GraphicsContext.h"
#include "Quark/Renderer/Renderer.h"

#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain final : public SwapChain
	{
	public:
		VulkanSwapChain(GLFWwindow* window, vk::SurfaceKHR surface);
		~VulkanSwapChain();

		void Init();
		void Present(vk::Queue presentQueue);
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		uint32_t AcquireNextImageIndex(vk::Semaphore imageAvailableSemaphore);

		vk::ImageView GetImageView(uint32_t i) const { return m_SwapChainImageViews[i]; }
		vk::Fence     GetFence() const { return m_InFlightFences[m_CurrentFrameIndex]; }
		vk::Semaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentFrameIndex]; }

		const SwapChainSpecification& GetSpecification() const { return m_Spec; }

	private:
		void Invalidate();

	private:
		GLFWwindow* m_WindowHandle;
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		vk::SurfaceFormatKHR m_ActualSurfaceFormat;
		vk::PresentModeKHR m_ActualPresentMode{};

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VkFence m_InFlightFences[Renderer::FramesInFlight]{};
		VkSemaphore m_RenderFinishedSemaphores[Renderer::FramesInFlight]{};

		uint32_t m_ImageIndex = 0;
		uint32_t m_CurrentFrameIndex = std::numeric_limits<uint32_t>::max();

		SwapChainSpecification m_Spec;

		// TODO: remove
		friend class VulkanRenderPass;
	};
}

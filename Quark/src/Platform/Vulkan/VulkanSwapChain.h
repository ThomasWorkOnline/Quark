#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(GLFWwindow* window, vk::SurfaceKHR surface);
		~VulkanSwapChain();

		uint32_t GetWidth() const { return m_Format.Extent.width; }
		uint32_t GetHeight() const { return m_Format.Extent.height; }
		uint32_t GetImageCount() const { return m_Format.ImageCount; }

		void AcquireNextImageIndex();
		void Present(vk::Queue presentQueue);
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }
		VkImageView GetImageView(uint32_t i) const { return m_SwapChainImageViews[i]; }

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
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		uint32_t m_ImageIndex = 0;
		Format m_Format;

		// TODO: remove
		friend class VulkanRenderPass;
	};
}

#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Framebuffer.h"

#include <vulkan/vulkan.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(GLFWwindow* window, VkSurfaceKHR surface);
		~VulkanSwapChain();

		uint32_t GetWidth() const { return m_Format.Extent.width; }
		uint32_t GetHeight() const { return m_Format.Extent.height; }
		uint32_t GetImageCount() const { return m_Format.ImageCount; }
		uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }

		void AcquireNextImage();
		void Present();
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		const Ref<FramebufferAttachment>& GetAttachment(uint32_t imageIndex) const { return m_Attachments[imageIndex]; }

	private:
		void Invalidate();

	private:
		struct Format
		{
			uint32_t             ImageCount = 0;
			VkExtent2D           Extent{};
			VkSurfaceFormatKHR   ActualSurfaceFormat{};
			VkPresentModeKHR     ActualPresentMode{};
		};

		GLFWwindow* m_WindowHandle;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<Ref<FramebufferAttachment>> m_Attachments;

		uint32_t m_ImageIndex = 0;
		Format m_Format;
	};
}

#pragma once

#include "Quark/Core/Core.h"

#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"

#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, GLFWwindow* window, VkSurfaceKHR surface);
		~VulkanSwapChain();

		uint32_t GetWidth() const { return m_Format.Extent.width; }
		uint32_t GetHeight() const { return m_Format.Extent.height; }
		uint32_t GetImageCount() const { return m_ImageCount; }
		uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }

		bool AcquireNextImage(VkSemaphore imageAvailableSemaphore);
		void Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore);
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		VulkanFramebufferAttachment* GetAttachment(uint32_t imageIndex) { return &m_Attachments[imageIndex]; }

		// Non-Copyable
		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

	private:
		void Invalidate();

	private:
		struct Format
		{
			uint32_t             MinImageCount = 0;
			VkExtent2D           Extent{};
			VkSurfaceFormatKHR   SurfaceFormat{};
			VkPresentModeKHR     PresentMode{};
		};

		VulkanDevice* m_Device;
		GLFWwindow* m_WindowHandle;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VulkanFramebufferAttachment> m_Attachments;

		Format m_Format;
		uint32_t m_ImageCount;
		uint32_t m_ImageIndex = 0;
	};
}

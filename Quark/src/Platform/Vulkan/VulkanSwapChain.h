#pragma once

#include "Quark/Core/Core.h"

#include "VulkanDevice.h"
#include <vulkan/vulkan.h>

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VulkanDevice* device, void* window, VkSurfaceKHR surface);
		~VulkanSwapChain();

		uint32_t GetWidth() const { return m_Format.Extent.width; }
		uint32_t GetHeight() const { return m_Format.Extent.height; }
		uint32_t GetImageCount() const { return m_ImageCount; }
		uint32_t GetCurrentImageIndex() const { return m_ImageIndex; }

		void AcquireNextImage(VkSemaphore imageAvailableSemaphore);
		void Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore);
		void Resize(uint32_t viewportWidth, uint32_t viewportHeight);

		VkImageView GetImageView(uint32_t imageIndex) const { return m_Attachments[imageIndex]; }
		VkImageView* GetImageViews() { return m_Attachments.data(); }

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
		std::vector<VkImageView> m_Attachments;

		Format m_Format;
		uint32_t m_ImageCount;
		uint32_t m_ImageIndex = 0;
	};
}

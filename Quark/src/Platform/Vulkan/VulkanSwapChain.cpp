#include "qkpch.h"
#include "VulkanSwapChain.h"

#include <GLFW/glfw3.h>

namespace Quark {

	namespace Utils {

		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					return availableFormat;
			}

			QK_CORE_WARN("Swap surface format not found: using default format 0");
			return availableFormats[0];
		}

		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return availablePresentMode;
			}

			QK_CORE_WARN("Swap present mode not found: using default FIFO present mode");
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				VkExtent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}
	}

	VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, GLFWwindow* window, VkSurfaceKHR surface)
		: m_Device(device), m_WindowHandle(window), m_Surface(surface)
	{
		auto& supportDetails = m_Device->GetSupportDetails();

		m_Format.MinImageCount = supportDetails.Capabilities.minImageCount + 1;
		if (m_Format.MinImageCount > supportDetails.Capabilities.maxImageCount)
			m_Format.MinImageCount = supportDetails.Capabilities.maxImageCount;

		m_Format.SurfaceFormat = Utils::ChooseSwapSurfaceFormat(supportDetails.Formats);
		m_Format.PresentMode = Utils::ChooseSwapPresentMode(supportDetails.PresentModes);
		m_Format.Extent = Utils::ChooseSwapExtent(supportDetails.Capabilities, m_WindowHandle);

		Invalidate();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		vkDestroySwapchainKHR(m_Device->GetVkHandle(), m_SwapChain, nullptr);
	}

	void VulkanSwapChain::AcquireNextImage(VkSemaphore imageAvailableSemaphore)
	{
		VkResult vkRes = vkAcquireNextImageKHR(m_Device->GetVkHandle(), m_SwapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not acquire next image in swap chain");
	}

	void VulkanSwapChain::Present(VkQueue presentQueue, VkSemaphore renderFinishedSemaphore)
	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_ImageIndex;

		VkResult vkRes = vkQueuePresentKHR(presentQueue, &presentInfo);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS || vkRes == VK_SUBOPTIMAL_KHR, "Could not present");
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		if (viewportWidth == 0 || viewportHeight == 0) return;

		if (m_Format.Extent.width != viewportWidth || m_Format.Extent.height != viewportHeight)
		{
			m_Format.Extent.width = viewportWidth;
			m_Format.Extent.height = viewportHeight;

			Invalidate();
		}
	}

	void VulkanSwapChain::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		bool isNew = !m_SwapChain;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = m_Format.MinImageCount;
		createInfo.imageFormat = m_Format.SurfaceFormat.format;
		createInfo.imageColorSpace = m_Format.SurfaceFormat.colorSpace;
		createInfo.imageExtent = m_Format.Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_Format.PresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = m_SwapChain;

		uint32_t queueFamilyIndices[] = {
			m_Device->GetQueueFamilyIndices().GraphicsFamily.value(),
			m_Device->GetQueueFamilyIndices().PresentFamily.value()
		};

		if (m_Device->GetQueueFamilyIndices().GraphicsFamily != m_Device->GetQueueFamilyIndices().PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = sizeof(queueFamilyIndices);
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
		}

		vkCreateSwapchainKHR(m_Device->GetVkHandle(), &createInfo, nullptr, &m_SwapChain);
		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &m_ImageCount, nullptr);

		m_SwapChainImages.resize(m_ImageCount);
		m_Attachments.resize(m_ImageCount);

		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &m_ImageCount, m_SwapChainImages.data());

		if (isNew)
		{
			for (size_t i = 0; i < m_ImageCount; i++)
			{
				m_Attachments[i] = CreateScope<VulkanFramebufferAttachment>(m_Device, m_SwapChainImages[i], m_Format.SurfaceFormat.format);
			}
		}
		else
		{
			for (size_t i = 0; i < m_ImageCount; i++)
			{
				m_Attachments[i]->SetData(m_SwapChainImages[i]);
			}
		}
	}
}

#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanEnums.h"

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, const SwapChainSpecification& spec)
		: SwapChain(spec), m_Device(device), m_Surface(surface)
	{
		Invalidate();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		vkDestroySwapchainKHR(m_Device->GetVkHandle(), m_SwapChain, nullptr);
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		if (viewportWidth == 0 || viewportHeight == 0) return;

		if (m_Spec.Extent.Width != viewportWidth || m_Spec.Extent.Height != viewportHeight)
		{
			m_Spec.Extent.Width = viewportWidth;
			m_Spec.Extent.Height = viewportHeight;

			Invalidate();
		}
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

		vkQueuePresentKHR(presentQueue, &presentInfo);
	}

	VkResult VulkanSwapChain::AcquireNextImage(VkSemaphore imageAvailableSemaphore)
	{
		return vkAcquireNextImageKHR(m_Device->GetVkHandle(), m_SwapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
	}

	void VulkanSwapChain::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType                     = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface                   = m_Surface;
		createInfo.minImageCount             = m_Spec.MinImageCount;
		createInfo.imageFormat               = DataFormatToVulkan(m_Spec.SurfaceFormat.Format);
		createInfo.imageColorSpace           = (VkColorSpaceKHR)m_Spec.SurfaceFormat.ColorSpace;
		createInfo.imageExtent               = VkExtent2D{ m_Spec.Extent.Width, m_Spec.Extent.Height };
		createInfo.imageArrayLayers          = 1;
		createInfo.imageUsage                = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform              = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha            = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode               = (VkPresentModeKHR)m_Spec.PresentMode;
		createInfo.clipped                   = VK_TRUE;
		createInfo.oldSwapchain              = m_SwapChain;

		uint32_t queueFamilyIndices[] = {
			m_Device->GetQueueFamilyIndices().GraphicsFamily.value(),
			m_Device->GetQueueFamilyIndices().PresentFamily.value()
		};

		if (m_Device->GetQueueFamilyIndices().GraphicsFamily != m_Device->GetQueueFamilyIndices().PresentFamily)
		{
			createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = sizeof_array(queueFamilyIndices);
			createInfo.pQueueFamilyIndices   = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
		}

		bool isNew = !m_SwapChain;
		vkCreateSwapchainKHR(m_Device->GetVkHandle(), &createInfo, nullptr, &m_SwapChain);

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &imageCount, nullptr);

		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &imageCount, m_SwapChainImages.data());
	}
}

#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanContext.h"
#include "VulkanRenderer.h"

#include <GLFW/glfw3.h>

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(GLFWwindow* window, VkSurfaceKHR surface)
		: m_WindowHandle(window), m_Surface(surface)
	{
		auto& supportDetails = VulkanContext::GetCurrentDevice()->GetSupportDetails();

		m_Format.ImageCount = supportDetails.Capabilities.minImageCount + 1;
		if (supportDetails.Capabilities.maxImageCount > 0 && m_Format.ImageCount > supportDetails.Capabilities.maxImageCount)
			m_Format.ImageCount = supportDetails.Capabilities.maxImageCount;

		m_Format.ActualSurfaceFormat = Utils::ChooseSwapSurfaceFormat(supportDetails.Formats);
		m_Format.ActualPresentMode = Utils::ChooseSwapPresentMode(supportDetails.PresentModes);
		m_Format.Extent = Utils::ChooseSwapExtent(supportDetails.Capabilities, m_WindowHandle);

		Invalidate();

		FramebufferAttachmentSpecification attachmentSpec;
		attachmentSpec.DataFormat = ColorDataFormat::BRGA8_SRGB;
		for (size_t i = 0; i < m_Format.ImageCount; i++)
		{
			m_Attachments[i] = FramebufferAttachment::Create(m_SwapChainImages[i], attachmentSpec);
		}
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroySwapchainKHR(vkDevice, m_SwapChain, nullptr);
	}

	void VulkanSwapChain::AcquireNextImage()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		VkResult vkRes = vkAcquireNextImageKHR(vkDevice, m_SwapChain, UINT64_MAX, VulkanRenderer::GetImageAvailableSemaphore(), VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not acquire next image in swap chain");
	}

	void VulkanSwapChain::Present(void* presentQueue)
	{
		auto renderFinishedSemaphore = VulkanRenderer::GetRenderFinishedSemaphore();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_ImageIndex;

		VkResult vkRes = vkQueuePresentKHR(static_cast<VkQueue>(presentQueue), &presentInfo);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS || vkRes == VK_SUBOPTIMAL_KHR, "Could not present");
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		VulkanContext::GetCurrentDevice()->WaitIdle();

		// FIX: this will fail if extent dimensions is zero
		if (m_Format.Extent.width != viewportWidth || m_Format.Extent.height != viewportHeight)
		{
			m_Format.Extent.width = viewportWidth;
			m_Format.Extent.height = viewportHeight;

			Invalidate();
		}
	}

	// IMPROVE: recreate swap chain using existing memory
	void VulkanSwapChain::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		bool isNew = m_SwapChain == VK_NULL_HANDLE;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = m_Format.ImageCount;
		createInfo.imageFormat = m_Format.ActualSurfaceFormat.format;
		createInfo.imageColorSpace = m_Format.ActualSurfaceFormat.colorSpace;
		createInfo.imageExtent = m_Format.Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_Format.ActualPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = m_SwapChain;

		auto device = VulkanContext::GetCurrentDevice();
		uint32_t queueFamilyIndices[] = {
			device->GetQueueFamilyIndices().GraphicsFamily.value(),
			device->GetQueueFamilyIndices().PresentFamily.value()
		};

		if (device->GetQueueFamilyIndices().GraphicsFamily != device->GetQueueFamilyIndices().PresentFamily)
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

		vkCreateSwapchainKHR(device->GetVkHandle(), &createInfo, nullptr, &m_SwapChain);
		vkGetSwapchainImagesKHR(device->GetVkHandle(), m_SwapChain, &m_Format.ImageCount, nullptr);

		m_SwapChainImages.resize(m_Format.ImageCount);
		m_Attachments.resize(m_Format.ImageCount);

		vkGetSwapchainImagesKHR(device->GetVkHandle(), m_SwapChain, &m_Format.ImageCount, m_SwapChainImages.data());

		if (!isNew)
		{
			for (size_t i = 0; i < m_Format.ImageCount; i++)
			{
				m_Attachments[i]->SetData(m_SwapChainImages[i]);
			}
		}
	}
}

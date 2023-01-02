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

		if (m_Spec.Width != viewportWidth || m_Spec.Height != viewportHeight)
		{
			m_Spec.Width = viewportWidth;
			m_Spec.Height = viewportHeight;

			Invalidate();
		}
	}

	void VulkanSwapChain::SetPresentMode(SwapPresentMode presentMode)
	{
		if (m_Spec.PresentMode == presentMode)
			return;

		m_Spec.PresentMode = presentMode;

		Invalidate();
	}

	uint32_t VulkanSwapChain::GetBufferCount() const
	{
		return static_cast<uint32_t>(m_ColorAttachments.size());
	}

	const FramebufferAttachment* VulkanSwapChain::GetColorAttachment(uint32_t index) const
	{
		QK_CORE_ASSERT(index < m_ColorAttachments.size(), "Index out of bounds");
		return m_ColorAttachments[index].get();
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
		createInfo.imageExtent               = VkExtent2D{ m_Spec.Width, m_Spec.Height };
		createInfo.imageArrayLayers          = 1;
		createInfo.imageUsage                = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform              = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha            = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode               = PresentModeToVulkan(m_Spec.PresentMode);
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

		bool isNew = m_SwapChain == nullptr;
		vkCreateSwapchainKHR(m_Device->GetVkHandle(), &createInfo, nullptr, &m_SwapChain);

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &imageCount, nullptr);

		m_ColorAttachments.resize(imageCount);

		AutoRelease<VkImage> images = StackAlloc(imageCount * sizeof(VkImage));
		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &imageCount, images.Data());

		if (isNew)
		{
			// Color buffers
			FramebufferAttachmentSpecification colorAttachmentSpec;
			colorAttachmentSpec.Width = m_Spec.Width;
			colorAttachmentSpec.Height = m_Spec.Height;
			colorAttachmentSpec.Samples = m_Spec.Samples;
			colorAttachmentSpec.DataFormat = m_Spec.SurfaceFormat.Format;

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
				m_ColorAttachments[i] = CreateScope<VulkanFramebufferAttachment>(m_Device, images[i], colorAttachmentSpec);
		}
		else
		{
			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				m_ColorAttachments[i]->SetSwapChainImage(images[i]);
				m_ColorAttachments[i]->Resize(m_Spec.Width, m_Spec.Height);
			}
		}
	}
}

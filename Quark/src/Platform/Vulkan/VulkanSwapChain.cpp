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
		return static_cast<uint32_t>(m_SwapChainImages.size());
	}

	Ref<FramebufferAttachment> VulkanSwapChain::GetColorAttachment(uint32_t index) const
	{
		QK_CORE_ASSERT(index < m_ColorAttachments.size(), "Index out of bounds");
		return m_ColorAttachments[index];
	}

	Ref<FramebufferAttachment> VulkanSwapChain::GetDepthAttachment(uint32_t index) const
	{
		QK_CORE_ASSERT(index < m_DepthAttachments.size(), "Index out of bounds");
		return m_DepthAttachments[index];
	}

	Ref<FramebufferAttachment> VulkanSwapChain::GetResolveAttachment(uint32_t index) const
	{
		QK_CORE_ASSERT(index < m_ResolveAttachments.size(), "Index out of bounds");
		return m_ResolveAttachments[index];
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

		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetVkHandle(), m_SwapChain, &imageCount, m_SwapChainImages.data());

		if (isNew)
		{
			// Color buffers
			{
				m_ColorAttachments.resize(imageCount);

				FramebufferAttachmentSpecification colorAttachmentSpec;
				colorAttachmentSpec.Width = m_Spec.Width;
				colorAttachmentSpec.Height = m_Spec.Height;
				colorAttachmentSpec.Samples = m_Spec.Samples;
				colorAttachmentSpec.DataFormat = m_Spec.SurfaceFormat.Format;
				colorAttachmentSpec.SwapChainTarget = true;

				for (size_t i = 0; i < m_ColorAttachments.size(); i++)
					m_ColorAttachments[i] = CreateRef<VulkanFramebufferAttachment>(m_Device, m_SwapChainImages[i], colorAttachmentSpec);
			}

			// Resolve buffers
			if (m_Spec.Samples > SampleCount::SampleCount1)
			{
				m_ResolveAttachments.resize(imageCount);

				FramebufferAttachmentSpecification resolveAttachmentSpec;
				resolveAttachmentSpec.Width = m_Spec.Width;
				resolveAttachmentSpec.Height = m_Spec.Height;
				resolveAttachmentSpec.Samples = m_Spec.Samples;
				resolveAttachmentSpec.DataFormat = m_Spec.SurfaceFormat.Format;

				for (size_t i = 0; i < m_ResolveAttachments.size(); i++)
					m_ResolveAttachments[i] = CreateRef<VulkanFramebufferAttachment>(m_Device, resolveAttachmentSpec);
			}

			// Depth buffers
			if (m_Spec.DepthBufferFormat != ColorFormat::None)
			{
				m_DepthAttachments.resize(imageCount);

				FramebufferAttachmentSpecification depthAttachmentSpec;
				depthAttachmentSpec.Width = m_Spec.Width;
				depthAttachmentSpec.Height = m_Spec.Height;
				depthAttachmentSpec.Samples = m_Spec.Samples;
				depthAttachmentSpec.DataFormat = m_Spec.DepthBufferFormat;

				for (size_t i = 0; i < m_DepthAttachments.size(); i++)
					m_DepthAttachments[i] = CreateRef<VulkanFramebufferAttachment>(m_Device, depthAttachmentSpec);
			}
		}
		else
		{
			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
				m_ColorAttachments[i]->SetSwapChainImage(m_SwapChainImages[i]);
		}
	}
}

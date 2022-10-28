#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanEnums.h"
#include "VulkanContext.h"

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, VkSurfaceKHR surface, const SwapChainSpecification& spec)
		: m_Device(device), m_Surface(surface), m_Spec(spec)
	{
		QK_CORE_ASSERT(spec.RenderPass, "RenderPass must be a valid pointer to a render pass");

		Invalidate();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		vkDestroySwapchainKHR(m_Device->GetVkHandle(), m_SwapChain, nullptr);
	}

	VkResult VulkanSwapChain::AcquireNextImage(VkSemaphore imageAvailableSemaphore)
	{
		return vkAcquireNextImageKHR(m_Device->GetVkHandle(), m_SwapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
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

	Framebuffer* VulkanSwapChain::GetFramebuffer() const
	{
		return m_Framebuffers[m_ImageIndex].get();
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

		bool invalidated = isNew || imageCount != m_Framebuffers.size();

		if (invalidated)
		{
			m_Framebuffers.clear();
			m_Framebuffers.reserve(imageCount);

			for (uint32_t i = 0; i < imageCount; i++)
			{
				FramebufferSpecification fbSpec;
				fbSpec.Width = m_Spec.Extent.Width;
				fbSpec.Height = m_Spec.Extent.Height;
				fbSpec.RenderPass = m_Spec.RenderPass;
				fbSpec.SwapChainTarget = true;

				if (m_Spec.RenderPass->GetSpecification().Samples > 1)
				{
					FramebufferAttachmentSpecification resolveAttachmentSpec;
					resolveAttachmentSpec.Width = m_Spec.Extent.Width;
					resolveAttachmentSpec.Height = m_Spec.Extent.Height;
					resolveAttachmentSpec.Samples = m_Spec.RenderPass->GetSpecification().Samples;
					resolveAttachmentSpec.DataFormat = m_Spec.SurfaceFormat.Format;

					fbSpec.Attachments.push_back(CreateRef<VulkanFramebufferAttachment>(m_Device, resolveAttachmentSpec));
				}

				{
					FramebufferAttachmentSpecification colorAttachmentSpec;
					colorAttachmentSpec.Width = m_Spec.Extent.Width;
					colorAttachmentSpec.Height = m_Spec.Extent.Height;
					colorAttachmentSpec.Samples = 1;
					colorAttachmentSpec.DataFormat = m_Spec.SurfaceFormat.Format;

					fbSpec.Attachments.push_back(CreateRef<VulkanFramebufferAttachment>(m_Device, colorAttachmentSpec, m_SwapChainImages[i]));
				}

				if (m_Spec.RenderPass->GetSpecification().DepthAttachmentFormat != ColorFormat::None)
				{
					FramebufferAttachmentSpecification depthAttachmentSpec;
					depthAttachmentSpec.Width = m_Spec.Extent.Width;
					depthAttachmentSpec.Height = m_Spec.Extent.Height;
					depthAttachmentSpec.Samples = m_Spec.RenderPass->GetSpecification().Samples;
					depthAttachmentSpec.DataFormat = m_Spec.RenderPass->GetSpecification().DepthAttachmentFormat;

					fbSpec.Attachments.push_back(CreateRef<VulkanFramebufferAttachment>(m_Device, depthAttachmentSpec));
				}

				m_Framebuffers.push_back(CreateScope<VulkanFramebuffer>(m_Device, fbSpec));
			}
		}
		else
		{
			for (uint32_t i = 0; i < imageCount; i++)
			{
				auto* vulkanAttachment = static_cast<VulkanFramebufferAttachment*>(m_Framebuffers[i]->GetSpecification().Attachments[1].get());
				vulkanAttachment->SetSwapChainImage(m_SwapChainImages[i]);
				m_Framebuffers[i]->Resize(m_Spec.Extent.Width, m_Spec.Extent.Height);
			}
		}
	}
}

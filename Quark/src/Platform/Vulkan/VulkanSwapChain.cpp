#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanEnums.h"
#include "VulkanContext.h"

namespace Quark {

	namespace Utils {

		static ColorFormat VulkanFormatToColorFormat(VkFormat format)
		{
			switch (format)
			{
				case VK_FORMAT_R8G8B8_UNORM:        return ColorFormat::RGB8;
				case VK_FORMAT_R16G16B16_UNORM:     return ColorFormat::RGB16;
				case VK_FORMAT_R32G32B32_UINT:      return ColorFormat::RGB32UInt;

				case VK_FORMAT_R8G8B8A8_UNORM:      return ColorFormat::RGBA8;
				case VK_FORMAT_R16G16B16A16_UNORM:  return ColorFormat::RGBA16;

				case VK_FORMAT_R8G8B8_SRGB:         return ColorFormat::RGB8SRGB;
				case VK_FORMAT_R8G8B8A8_SRGB:       return ColorFormat::RGBA8SRGB;

				case VK_FORMAT_B8G8R8_SRGB:         return ColorFormat::BGR8SRGB;
				case VK_FORMAT_B8G8R8A8_SRGB:       return ColorFormat::BGRA8SRGB;

				case VK_FORMAT_R16G16B16_SFLOAT:    return ColorFormat::RGB16f;
				case VK_FORMAT_R32G32B32_SFLOAT:    return ColorFormat::RGB32f;
				case VK_FORMAT_R16G16B16A16_SFLOAT: return ColorFormat::RGBA16f;
				case VK_FORMAT_R32G32B32A32_SFLOAT: return ColorFormat::RGBA32f;

				case VK_FORMAT_R8_UNORM:            return ColorFormat::Red8;

				case VK_FORMAT_D16_UNORM:           return ColorFormat::Depth16;
				case VK_FORMAT_D24_UNORM_S8_UINT:   return ColorFormat::Depth24Stencil8;

				case VK_FORMAT_D32_SFLOAT:          return ColorFormat::Depth32f;

				QK_ASSERT_NO_DEFAULT("Unknown VkFormat");
			}

			return ColorFormat::None;
		}
	}

	VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, const VulkanSwapChainSpecification& spec)
		: m_Device(device), m_Spec(spec)
	{
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

		if (m_Spec.Extent.width != viewportWidth || m_Spec.Extent.height != viewportHeight)
		{
			m_Spec.Extent.width = viewportWidth;
			m_Spec.Extent.height = viewportHeight;

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
		createInfo.surface                   = m_Spec.Surface;
		createInfo.minImageCount             = m_Spec.MinImageCount;
		createInfo.imageFormat               = m_Spec.SurfaceFormat.format;
		createInfo.imageColorSpace           = m_Spec.SurfaceFormat.colorSpace;
		createInfo.imageExtent               = m_Spec.Extent;
		createInfo.imageArrayLayers          = 1;
		createInfo.imageUsage                = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform              = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha            = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode               = m_Spec.PresentMode;
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

			FramebufferAttachmentSpecification colorAttachmentSpec;
			colorAttachmentSpec.DataFormat = Utils::VulkanFormatToColorFormat(m_Spec.SurfaceFormat.format);
			colorAttachmentSpec.Width = m_Spec.Extent.width;
			colorAttachmentSpec.Height = m_Spec.Extent.height;
			colorAttachmentSpec.Samples = 1;

			FramebufferAttachmentSpecification depthAttachmentSpec;
			depthAttachmentSpec.DataFormat = ColorFormat::Depth32f;
			depthAttachmentSpec.Width = m_Spec.Extent.width;
			depthAttachmentSpec.Height = m_Spec.Extent.height;
			depthAttachmentSpec.Samples = 1;

			FramebufferSpecification fbSpec;
			fbSpec.Width = m_Spec.Extent.width;
			fbSpec.Height = m_Spec.Extent.height;
			fbSpec.Samples = 1;
			fbSpec.RenderPass = m_Spec.RenderPass;
			fbSpec.SwapChainTarget = true;

			for (uint32_t i = 0; i < imageCount; i++)
			{
				fbSpec.Attachments = {
					CreateRef<VulkanFramebufferAttachment>(m_Device, m_SwapChainImages[i], colorAttachmentSpec),
					CreateRef<VulkanFramebufferAttachment>(m_Device, nullptr, depthAttachmentSpec)
				};

				m_Framebuffers.push_back(CreateScope<VulkanFramebuffer>(m_Device, fbSpec));
			}
		}
		else
		{
			for (uint32_t i = 0; i < imageCount; i++)
			{
				auto* vulkanAttachment = static_cast<VulkanFramebufferAttachment*>(m_Framebuffers[i]->GetSpecification().Attachments[0].get());
				vulkanAttachment->SetSwapChainImage(m_SwapChainImages[i]);
				m_Framebuffers[i]->Resize(m_Spec.Extent.width, m_Spec.Extent.height);
			}
		}
	}
}

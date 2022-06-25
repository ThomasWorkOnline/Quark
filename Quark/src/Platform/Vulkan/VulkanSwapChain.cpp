#include "qkpch.h"
#include "VulkanSwapChain.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(vk::SurfaceKHR surface, const VulkanSwapChainSpecification& spec)
		: m_VkDevice(VulkanGraphicsContext::GetCurrentDevice().GetVkHandle()), m_VkSurface(surface), m_Spec(spec)
	{
		Invalidate(m_Spec.Extent.width, m_Spec.Extent.height);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		m_VkPresentQueue.waitIdle();
		for (uint32_t i = 0; i < g_FramesInFlight; i++)
		{
			m_VkDevice.destroySemaphore(m_VkRenderFinishedSemaphores[i]);
			m_VkDevice.destroySemaphore(m_VkImageAvailableSemaphores[i]);
		}

		for (uint32_t i = 0; i < m_VkSwapChainImageViews.size(); i++)
			m_VkDevice.destroyImageView(m_VkSwapChainImageViews[i]);

		m_VkDevice.destroySwapchainKHR(m_VkSwapChain);
	}

	void VulkanSwapChain::Present()
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.setWaitSemaphoreCount(1);
		presentInfo.setPWaitSemaphores(&m_VkRenderFinishedSemaphores[m_CurrentFrameIndex]);

		presentInfo.setSwapchainCount(1);
		presentInfo.setPSwapchains(&m_VkSwapChain);
		presentInfo.setPImageIndices(&m_ImageIndex);

		vk::Result vkRes = m_VkPresentQueue.presentKHR(presentInfo);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not present");
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		Invalidate(viewportWidth, viewportHeight);
	}

	uint32_t VulkanSwapChain::AcquireNextImageIndex(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;
		vk::Result vkRes = m_VkDevice.acquireNextImageKHR(m_VkSwapChain, UINT64_MAX, m_VkImageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not acquire next image in swap chain");

		return m_ImageIndex;
	}

	void VulkanSwapChain::Invalidate(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_PROFILE_FUNCTION();

		if (m_Spec.Extent.width != viewportWidth || m_Spec.Extent.height != viewportHeight)
		{
			m_VkPresentQueue.waitIdle();
			for (uint32_t i = 0; i < m_VkSwapChainImageViews.size(); i++)
				m_VkDevice.destroyImageView(m_VkSwapChainImageViews[i]);

			m_VkDevice.destroySwapchainKHR(m_VkSwapChain);

			m_Spec.Extent.width = viewportWidth;
			m_Spec.Extent.height = viewportHeight;
		}
		else
		{
			m_VkRenderFinishedSemaphores.resize(g_FramesInFlight);
			m_VkImageAvailableSemaphores.resize(g_FramesInFlight);

			vk::SemaphoreCreateInfo semaphoreInfo;
			for (uint32_t i = 0; i < g_FramesInFlight; i++)
			{
				m_VkRenderFinishedSemaphores[i] = m_VkDevice.createSemaphore(semaphoreInfo);
				m_VkImageAvailableSemaphores[i] = m_VkDevice.createSemaphore(semaphoreInfo);
			}
		}

		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(VK_TRUE);
		createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
		createInfo.setImageExtent(m_Spec.Extent);
		createInfo.setImageFormat(m_Spec.SurfaceFormat.format);
		createInfo.setImageColorSpace(m_Spec.SurfaceFormat.colorSpace);
		createInfo.setImageArrayLayers(1);
		createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		createInfo.setMinImageCount(m_Spec.ImageCount);
		createInfo.setOldSwapchain(VK_NULL_HANDLE);
		createInfo.setPresentMode(m_Spec.PresentMode);
		createInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
		createInfo.setSurface(m_VkSurface);

		uint32_t queueFamilyIndices[] = {
			*m_Spec.FamilyIndices.GraphicsFamily,
			*m_Spec.FamilyIndices.PresentFamily
		};

		if (m_Spec.FamilyIndices.GraphicsFamily != m_Spec.FamilyIndices.PresentFamily)
		{
			createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
			createInfo.setQueueFamilyIndexCount(sizeof(queueFamilyIndices));
			createInfo.setPQueueFamilyIndices(queueFamilyIndices);
		}
		else
		{
			createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
			createInfo.setQueueFamilyIndexCount(0);
		}

		m_VkPresentQueue = m_VkDevice.getQueue(*m_Spec.FamilyIndices.PresentFamily, 0);

		m_VkSwapChain = m_VkDevice.createSwapchainKHR(createInfo);
		m_VkSwapChainImages = m_VkDevice.getSwapchainImagesKHR(m_VkSwapChain);

		m_VkSwapChainImageViews.resize(m_VkSwapChainImages.size());
		for (size_t i = 0; i < m_VkSwapChainImages.size(); i++)
		{
			vk::ImageViewCreateInfo createInfo;
			createInfo.setImage(m_VkSwapChainImages[i]);
			createInfo.setViewType(vk::ImageViewType::e2D);
			createInfo.setFormat(m_Spec.SurfaceFormat.format);
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			m_VkSwapChainImageViews[i] = m_VkDevice.createImageView(createInfo);
		}
	}
}

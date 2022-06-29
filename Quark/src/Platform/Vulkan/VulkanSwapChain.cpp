#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanContext.h"

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(vk::SurfaceKHR surface, const VulkanSwapChainSpecification& spec)
		: m_Surface(surface), m_Spec(spec)
	{
		// Synchronization
		{
			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			m_RenderFinishedSemaphores.resize(g_FramesInFlight);

			vk::SemaphoreCreateInfo semaphoreInfo;
			for (uint32_t i = 0; i < g_FramesInFlight; i++)
				m_RenderFinishedSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
		}

		Invalidate(m_Spec.Extent.width, m_Spec.Extent.height);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		for (uint32_t i = 0; i < g_FramesInFlight; i++)
			vkDevice.destroySemaphore(m_RenderFinishedSemaphores[i]);

		for (uint32_t i = 0; i < m_SwapChainImageViews.size(); i++)
			vkDevice.destroyImageView(m_SwapChainImageViews[i]);

		vkDevice.destroySwapchainKHR(m_SwapChain);
	}

	void VulkanSwapChain::Present(vk::Queue presentQueue)
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.setWaitSemaphoreCount(1);
		presentInfo.setPWaitSemaphores(&m_RenderFinishedSemaphores[m_CurrentFrameIndex]);

		presentInfo.setSwapchainCount(1);
		presentInfo.setPSwapchains(&m_SwapChain);
		presentInfo.setPImageIndices(&m_ImageIndex);

		vk::Result vkRes = presentQueue.presentKHR(presentInfo);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not present");
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		Invalidate(viewportWidth, viewportHeight);
	}

	uint32_t VulkanSwapChain::AcquireNextImageIndex(vk::Semaphore imageAvailableSemaphore)
	{
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % g_FramesInFlight;

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		vk::Result vkRes = vkDevice.acquireNextImageKHR(m_SwapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not acquire next image in swap chain");

		return m_ImageIndex;
	}

	void VulkanSwapChain::Invalidate(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		// FIX: this will fail if extent dimensions is zero
		if (m_Spec.Extent.width != viewportWidth || m_Spec.Extent.height != viewportHeight)
		{
			for (uint32_t i = 0; i < m_SwapChainImageViews.size(); i++)
				vkDevice.destroyImageView(m_SwapChainImageViews[i]);

			vkDevice.destroySwapchainKHR(m_SwapChain);

			m_Spec.Extent.width = viewportWidth;
			m_Spec.Extent.height = viewportHeight;
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
		createInfo.setSurface(m_Surface);

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

		m_SwapChain = vkDevice.createSwapchainKHR(createInfo);
		m_SwapChainImages = vkDevice.getSwapchainImagesKHR(m_SwapChain);

		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			vk::ImageViewCreateInfo createInfo;
			createInfo.setImage(m_SwapChainImages[i]);
			createInfo.setViewType(vk::ImageViewType::e2D);
			createInfo.setFormat(m_Spec.SurfaceFormat.format);
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			m_SwapChainImageViews[i] = vkDevice.createImageView(createInfo);
		}
	}
}

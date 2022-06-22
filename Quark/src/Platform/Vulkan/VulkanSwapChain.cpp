#include "qkpch.h"
#include "VulkanSwapChain.h"

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(vk::Device device, vk::SurfaceKHR surface, const VulkanSwapChainSpecification& spec)
		: m_VkDevice(device), m_VkSurface(surface), m_Spec(spec)
	{
		QK_PROFILE_FUNCTION();

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

		m_VkSwapChain = m_VkDevice.createSwapchainKHR(createInfo, nullptr);
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

			m_VkSwapChainImageViews[i] = m_VkDevice.createImageView(createInfo, nullptr);
		}

		m_VkRenderFinishedSemaphores.resize(g_FramesInFlight);
		m_VkImageAvailableSemaphores.resize(g_FramesInFlight);

		vk::SemaphoreCreateInfo semaphoreInfo;
		for (uint32_t i = 0; i < g_FramesInFlight; i++)
		{
			m_VkRenderFinishedSemaphores[i] = m_VkDevice.createSemaphore(semaphoreInfo, nullptr);
			m_VkImageAvailableSemaphores[i] = m_VkDevice.createSemaphore(semaphoreInfo, nullptr);
		}
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		m_VkPresentQueue.waitIdle();

		for (uint32_t i = 0; i < g_FramesInFlight; i++)
		{
			vkDestroySemaphore(m_VkDevice, m_VkRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_VkDevice, m_VkImageAvailableSemaphores[i], nullptr);
		}

		for (auto& imageView : m_VkSwapChainImageViews)
			vkDestroyImageView(m_VkDevice, imageView, nullptr);

		vkDestroySwapchainKHR(m_VkDevice, m_VkSwapChain, nullptr);
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

	uint32_t VulkanSwapChain::AcquireNextImageIndex(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;
		vk::Result vkRes = m_VkDevice.acquireNextImageKHR(m_VkSwapChain, UINT64_MAX, m_VkImageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == vk::Result::eSuccess, "Could not acquire next image in swap chain");

		return m_ImageIndex;
	}
}

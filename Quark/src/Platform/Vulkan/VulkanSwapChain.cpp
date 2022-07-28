#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanContext.h"
#include "VulkanRenderer.h"

#include <GLFW/glfw3.h>

namespace Quark {

	VulkanSwapChain::VulkanSwapChain(GLFWwindow* window, vk::SurfaceKHR surface)
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
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		for (auto& imageView : m_SwapChainImageViews)
			vkDestroyImageView(vkDevice, imageView, nullptr);

		vkDestroySwapchainKHR(vkDevice, m_SwapChain, nullptr);
	}

	void VulkanSwapChain::AcquireNextImageIndex()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

		// Wait for last frame to be complete
		{
			auto inFlightFence = VulkanRenderer::GetInFlightFence();

			VkResult vkRes = vkWaitForFences(vkDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not wait for fences");
			vkResetFences(vkDevice, 1, &inFlightFence);
		}

		VkResult vkRes = vkAcquireNextImageKHR(vkDevice, m_SwapChain, UINT64_MAX, VulkanRenderer::GetImageAvailableSemaphore(), VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not acquire next image in swap chain");
	}

	void VulkanSwapChain::Present(vk::Queue presentQueue)
	{
		auto renderFinishedSemaphore = VulkanRenderer::GetRenderFinishedSemaphore();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_ImageIndex;

		VkResult vkRes = vkQueuePresentKHR(presentQueue, &presentInfo);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not present");
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		{
			auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();

			Timer t;
			vkDeviceWaitIdle(vkDevice);
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

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

		{
			auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
			for (auto& imageView : m_SwapChainImageViews)
				vkDestroyImageView(vkDevice, imageView, nullptr);

			vkDestroySwapchainKHR(vkDevice, m_SwapChain, nullptr);
		}

		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(VK_TRUE);
		createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
		createInfo.setImageExtent(m_Format.Extent);
		createInfo.setImageFormat(m_Format.ActualSurfaceFormat.format);
		createInfo.setImageColorSpace(m_Format.ActualSurfaceFormat.colorSpace);
		createInfo.setImageArrayLayers(1);
		createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		createInfo.setMinImageCount(m_Format.ImageCount);
		createInfo.setOldSwapchain(VK_NULL_HANDLE);
		createInfo.setPresentMode(static_cast<vk::PresentModeKHR>(m_Format.ActualPresentMode));
		createInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
		createInfo.setSurface(m_Surface);

		auto device = VulkanContext::GetCurrentDevice();
		uint32_t queueFamilyIndices[] = {
			*device->GetQueueFamilyIndices().GraphicsFamily,
			*device->GetQueueFamilyIndices().PresentFamily
		};

		if (device->GetQueueFamilyIndices().GraphicsFamily != device->GetQueueFamilyIndices().PresentFamily)
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

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		m_SwapChain = vkDevice.createSwapchainKHR(createInfo);

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(vkDevice, m_SwapChain, &imageCount, nullptr);

		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(vkDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageViews.resize(imageCount);
		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = (VkFormat)m_Format.ActualSurfaceFormat.format;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			vkCreateImageView(vkDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]);
		}
	}
}

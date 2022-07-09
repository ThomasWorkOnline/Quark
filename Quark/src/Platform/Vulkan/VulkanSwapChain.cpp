#include "qkpch.h"
#include "VulkanSwapChain.h"

#include "VulkanContext.h"
#include <GLFW/glfw3.h>

namespace Quark {

	namespace Utils {

		static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
					return availableFormat;
			}

			QK_CORE_WARN("Swap surface format not found: using default format 0");
			return availableFormats[0];
		}

		static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == vk::PresentModeKHR::eMailbox)
					return availablePresentMode;
			}

			QK_CORE_WARN("Swap present mode not found: using default FIFO present mode");
			return vk::PresentModeKHR::eFifo;
		}

		static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				vk::Extent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}
	}

	VulkanSwapChain::VulkanSwapChain(GLFWwindow* window, vk::SurfaceKHR surface)
		: m_WindowHandle(window), m_Surface(surface)
	{
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		for (auto fence : m_InFlightFences)
			vkDestroyFence(vkDevice, fence, nullptr);

		for (auto semaphore : m_RenderFinishedSemaphores)
			vkDestroySemaphore(vkDevice, semaphore, nullptr);

		for (auto& imageView : m_SwapChainImageViews)
			vkDestroyImageView(vkDevice, imageView, nullptr);

		vkDestroySwapchainKHR(vkDevice, m_SwapChain, nullptr);
	}

	void VulkanSwapChain::Init()
	{
		// Specification
		{
			auto& supportDetails = VulkanContext::GetCurrentDevice().GetSupportDetails();
			m_ActualSurfaceFormat = Utils::ChooseSwapSurfaceFormat(supportDetails.Formats);
			m_ActualPresentMode = Utils::ChooseSwapPresentMode(supportDetails.PresentModes);
			auto extent = Utils::ChooseSwapExtent(supportDetails.Capabilities, m_WindowHandle);

			m_Spec.Width = extent.width;
			m_Spec.Height = extent.height;

			uint32_t imageCount = supportDetails.Capabilities.minImageCount + 1;
			if (supportDetails.Capabilities.maxImageCount > 0 && imageCount > supportDetails.Capabilities.maxImageCount)
				imageCount = supportDetails.Capabilities.maxImageCount;

			m_Spec.ImageCount = imageCount;
		}

		// Synchronization
		{
			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

			vk::FenceCreateInfo fenceInfo;
			fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

			vk::SemaphoreCreateInfo semaphoreInfo;
			for (uint32_t i = 0; i < Renderer::FramesInFlight; i++)
			{
				m_InFlightFences[i] = vkDevice.createFence(fenceInfo);
				m_RenderFinishedSemaphores[i] = vkDevice.createSemaphore(semaphoreInfo);
			}
		}

		Invalidate();
	}

	void VulkanSwapChain::Present(vk::Queue presentQueue)
	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_ImageIndex;

		VkResult vkRes = vkQueuePresentKHR(presentQueue, &presentInfo);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not present");
	}

	void VulkanSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		{
			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

			Timer t;
			vkDeviceWaitIdle(vkDevice);
			QK_CORE_INFO("Waiting for device to finish: {0}ms", t.Milliseconds().count());
		}

		// FIX: this will fail if extent dimensions is zero
		if (m_Spec.Width != viewportWidth || m_Spec.Height != viewportHeight)
		{
			m_Spec.Width = viewportWidth;
			m_Spec.Height = viewportHeight;

			Invalidate();
		}
	}

	uint32_t VulkanSwapChain::AcquireNextImageIndex(vk::Semaphore imageAvailableSemaphore)
	{
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Renderer::FramesInFlight;
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();

		// Wait for last frame to be complete
		{
			VkResult vkRes = vkWaitForFences(vkDevice, 1, &m_InFlightFences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX);
			QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not wait for fences");
			vkResetFences(vkDevice, 1, &m_InFlightFences[m_CurrentFrameIndex]);
		}

		VkResult vkRes = vkAcquireNextImageKHR(vkDevice, m_SwapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		QK_CORE_ASSERT(vkRes == VK_SUCCESS, "Could not acquire next image in swap chain");

		return m_ImageIndex;
	}

	// IMPROVE: recreate swap chain using existing memory
	void VulkanSwapChain::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		{
			auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
			for (auto& imageView : m_SwapChainImageViews)
				vkDestroyImageView(vkDevice, imageView, nullptr);

			vkDestroySwapchainKHR(vkDevice, m_SwapChain, nullptr);
		}

		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(VK_TRUE);
		createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
		createInfo.setImageExtent(vk::Extent2D{ m_Spec.Width, m_Spec.Height });
		createInfo.setImageFormat(m_ActualSurfaceFormat.format);
		createInfo.setImageColorSpace(m_ActualSurfaceFormat.colorSpace);
		createInfo.setImageArrayLayers(1);
		createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		createInfo.setMinImageCount(m_Spec.ImageCount);
		createInfo.setOldSwapchain(VK_NULL_HANDLE);
		createInfo.setPresentMode(static_cast<vk::PresentModeKHR>(m_Spec.PresentMode));
		createInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
		createInfo.setSurface(m_Surface);

		auto& device = VulkanContext::GetCurrentDevice();
		uint32_t queueFamilyIndices[] = {
			*device.GetQueueFamilyIndices().GraphicsFamily,
			*device.GetQueueFamilyIndices().PresentFamily
		};

		if (device.GetQueueFamilyIndices().GraphicsFamily != device.GetQueueFamilyIndices().PresentFamily)
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

		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
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
			createInfo.format = (VkFormat)m_ActualSurfaceFormat.format;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			vkCreateImageView(vkDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]);
		}
	}
}

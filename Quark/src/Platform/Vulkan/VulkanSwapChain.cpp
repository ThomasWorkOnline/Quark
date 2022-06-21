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
		createInfo.setImageExtent(spec.Extent);
		createInfo.setImageFormat(spec.SurfaceFormat.format);
		createInfo.setImageColorSpace(spec.SurfaceFormat.colorSpace);
		createInfo.setImageArrayLayers(1);
		createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		createInfo.setMinImageCount(spec.ImageCount);
		createInfo.setOldSwapchain(VK_NULL_HANDLE);
		createInfo.setPresentMode(spec.PresentMode);
		createInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
		createInfo.setSurface(m_VkSurface);

		uint32_t queueFamilyIndices[] = {
			*spec.FamilyIndices.GraphicsFamily,
			*spec.FamilyIndices.PresentFamily
		};

		if (spec.FamilyIndices.GraphicsFamily != spec.FamilyIndices.PresentFamily)
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

		m_VkSwapChain = m_VkDevice.createSwapchainKHR(createInfo, nullptr);
		m_VkSwapChainImages = m_VkDevice.getSwapchainImagesKHR(m_VkSwapChain);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		QK_PROFILE_FUNCTION();

		vkDestroySwapchainKHR(m_VkDevice, m_VkSwapChain, nullptr);
	}

	void VulkanSwapChain::SwapBuffers()
	{
	}
}

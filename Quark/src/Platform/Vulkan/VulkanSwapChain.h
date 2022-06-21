#pragma once

#include "VulkanCore.h"

namespace Quark {

	struct VulkanSwapChainSpecification
	{
		vk::SurfaceFormatKHR       SurfaceFormat{};
		vk::PresentModeKHR         PresentMode{};
		vk::Extent2D               Extent;

		Utils::QueueFamilyIndices  FamilyIndices;
		uint32_t                   ImageCount = 0;
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain() = default;
		VulkanSwapChain(vk::Device device, vk::SurfaceKHR surface, const VulkanSwapChainSpecification& spec);
		~VulkanSwapChain();

		void SwapBuffers();

	private:
		vk::Device m_VkDevice;
		vk::SurfaceKHR m_VkSurface;

		vk::SwapchainKHR m_VkSwapChain;
		std::vector<vk::Image> m_VkSwapChainImages;
		std::vector<vk::ImageView> m_VkSwapChainImageViews;

		VulkanSwapChainSpecification m_Spec;
	};
}

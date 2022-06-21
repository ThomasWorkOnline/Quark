#pragma once

#include "VulkanCore.h"

namespace Quark {

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(vk::Device device, vk::Format swapChainImageFormat);
		~VulkanRenderPass();

		vk::RenderPass GetNativeHandle() const { return m_VkRenderPass; }

	private:
		vk::Device m_VkDevice;
		vk::RenderPass m_VkRenderPass;
	};
}

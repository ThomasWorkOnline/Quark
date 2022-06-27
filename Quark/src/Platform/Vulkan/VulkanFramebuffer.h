#pragma once

#include "Quark/Renderer/Framebuffer.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		~VulkanFramebuffer();

		void Resize(uint32_t width, uint32_t height);

	private:
		void Invalidate();

	private:
		vk::Framebuffer m_VkFramebuffer;
		FramebufferSpecification m_Spec;
	};
}

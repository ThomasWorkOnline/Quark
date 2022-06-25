#pragma once

#include "Quark/Renderer/Framebuffer.h"
#include "VulkanCore.h"

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
		vk::Device m_VkDevice;
		vk::Framebuffer m_VkFramebuffer;

		FramebufferSpecification m_Spec;
	};
}

#include "qkpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace Quark {

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyFramebuffer(m_Framebuffer);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	void VulkanFramebuffer::Invalidate()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		if (m_Framebuffer)
		{
			vkDevice.destroyFramebuffer(m_Framebuffer);
		}
	}
}

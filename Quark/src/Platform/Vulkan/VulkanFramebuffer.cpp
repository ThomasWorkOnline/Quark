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
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyFramebuffer(vkDevice, m_Framebuffer, nullptr);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	void VulkanFramebuffer::Invalidate()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		if (m_Framebuffer)
		{
			vkDestroyFramebuffer(vkDevice, m_Framebuffer, nullptr);
		}
	}
}

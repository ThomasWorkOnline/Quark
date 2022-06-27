#include "qkpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice().GetVkHandle();
		vkDevice.destroyFramebuffer(m_VkFramebuffer);
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
		if (m_VkFramebuffer)
		{
			vkDevice.destroyFramebuffer(m_VkFramebuffer);
		}
	}
}

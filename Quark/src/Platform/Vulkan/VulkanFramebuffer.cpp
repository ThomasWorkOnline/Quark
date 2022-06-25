#include "qkpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsContext.h"

namespace Quark {

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_VkDevice(VulkanGraphicsContext::GetCurrentDevice().GetVkHandle()),
		m_Spec(spec)
	{
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		m_VkDevice.destroyFramebuffer(m_VkFramebuffer);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	void VulkanFramebuffer::Invalidate()
	{
		if (m_VkDevice)
		{
			m_VkDevice.destroyFramebuffer(m_VkFramebuffer);
		}
	}
}

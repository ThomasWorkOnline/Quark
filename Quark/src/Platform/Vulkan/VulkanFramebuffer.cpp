#include "qkpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanFormats.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.h"

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

namespace Quark {

	VulkanFramebufferAttachment::VulkanFramebufferAttachment(const FramebufferAttachmentSpecification& spec)
		: FramebufferAttachment(spec)
	{
	}

	VulkanFramebufferAttachment::~VulkanFramebufferAttachment()
	{
	}

	void VulkanFramebufferAttachment::SetData(void* data)
	{
	}

	VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const FramebufferSpecification& spec)
		: Framebuffer(spec)
		, m_Device(device)
	{
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		vkDestroyFramebuffer(m_Device->GetVkHandle(), m_Framebuffer, nullptr);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	void VulkanFramebuffer::Invalidate()
	{
		if (m_Framebuffer)
		{
			vkDestroyFramebuffer(m_Device->GetVkHandle(), m_Framebuffer, nullptr);
		}

		QK_CORE_ASSERT(m_Spec.AttachmentCount, "AttachmentCount must be non zero");
		QK_CORE_ASSERT(m_Spec.Attachments, "Attachments must be a valid pointer to an array of framebuffer attachments");
		QK_CORE_ASSERT(m_Spec.RenderPass, "RenderPass must be a valid pointer to a render pass");

		auto* attachments = static_cast<VkImageView*>(alloca(m_Spec.AttachmentCount * sizeof(VkImageView)));
		for (size_t i = 0; i < m_Spec.AttachmentCount; i++)
		{
			attachments[i] = static_cast<VkImageView>(m_Spec.Attachments[i]);
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = static_cast<VulkanRenderPass*>(m_Spec.RenderPass)->GetVkHandle();
		framebufferInfo.attachmentCount = m_Spec.AttachmentCount;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Spec.Width;
		framebufferInfo.height = m_Spec.Height;
		framebufferInfo.layers = 1;

		vkCreateFramebuffer(m_Device->GetVkHandle(), &framebufferInfo, nullptr, &m_Framebuffer);
	}
}

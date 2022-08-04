#include "qkpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanFormats.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.h"

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

namespace Quark {

	VulkanFramebufferAttachment::VulkanFramebufferAttachment(void* data, const FramebufferAttachmentSpecification& spec) : FramebufferAttachment(spec)
	{
	}

	VulkanFramebufferAttachment::~VulkanFramebufferAttachment()
	{
	}

	void VulkanFramebufferAttachment::SetData(void* data)
	{
	}

	VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const FramebufferSpecification& spec) : Framebuffer(spec),
		m_Device(device)
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

		auto* attachments = static_cast<VkImageView*>(alloca(m_Spec.Attachments.size() * sizeof(VkImageView)));
		for (size_t i = 0; i < m_Spec.Attachments.size(); i++)
		{
			QK_CORE_ASSERT(false, "");
			//attachments[i] = static_cast<VulkanFramebufferAttachment*>(m_Spec.Attachments[i].get())->GetImageView();
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = static_cast<VulkanRenderPass*>(m_Spec.RenderPass.get())->GetVkHandle();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(m_Spec.Attachments.size());
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Spec.Width;
		framebufferInfo.height = m_Spec.Height;
		framebufferInfo.layers = 1;

		vkCreateFramebuffer(m_Device->GetVkHandle(), &framebufferInfo, nullptr, &m_Framebuffer);
	}
}

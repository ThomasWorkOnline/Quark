#include "qkpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanFormats.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.h"

// disable alloca failure warning
// since variable size stack arrays are not supported by all compilers
#pragma warning(disable : 6255)

namespace Quark {

	VulkanFramebufferAttachment::VulkanFramebufferAttachment(VulkanDevice* device, const FramebufferAttachmentSpecification& spec)
		: FramebufferAttachment(spec), m_Device(device)
	{
	}

	VulkanFramebufferAttachment::VulkanFramebufferAttachment(VulkanDevice* device, VkImage image, VkFormat format)
		: m_Device(device), m_Format(format)
	{
		VkImageViewCreateInfo imageViewInfo{};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image = image;
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = m_Format;
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(m_Device->GetVkHandle(), &imageViewInfo, nullptr, &m_ImageView);
	}

	VulkanFramebufferAttachment::~VulkanFramebufferAttachment()
	{
		vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);
	}

	void VulkanFramebufferAttachment::SetData(const void* data)
	{
		if (m_ImageView)
			vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);

		VkImageViewCreateInfo imageViewInfo{};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image = (VkImage)data;
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = m_Format;
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(m_Device->GetVkHandle(), &imageViewInfo, nullptr, &m_ImageView);
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
		if (width == 0 || height == 0) return;

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

		QK_CORE_ASSERT(m_Spec.Attachments.size(), "AttachmentCount must be non zero");
		QK_CORE_ASSERT(m_Spec.RenderPass, "RenderPass must be a valid pointer to a render pass");

		auto* attachments = (VkImageView*)alloca(m_Spec.Attachments.size() * sizeof(VkImageView));
		auto* attachmentPtr = attachments;

		for (auto* attachment : m_Spec.Attachments)
		{
			*attachmentPtr = static_cast<VulkanFramebufferAttachment*>(attachment)->GetVkHandle();
			attachmentPtr++;
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = static_cast<VulkanRenderPass*>(m_Spec.RenderPass)->GetVkHandle();
		framebufferInfo.attachmentCount = (uint32_t)m_Spec.Attachments.size();
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Spec.Width;
		framebufferInfo.height = m_Spec.Height;
		framebufferInfo.layers = 1;

		vkCreateFramebuffer(m_Device->GetVkHandle(), &framebufferInfo, nullptr, &m_Framebuffer);
	}
}

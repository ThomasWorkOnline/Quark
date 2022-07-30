#include "qkpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanContext.h"
#include "VulkanFormats.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.h"

namespace Quark {

	VulkanFramebufferAttachment::VulkanFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec) : FramebufferAttachment(spec)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = static_cast<VkImage>(image);
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = InternalFormatToVulkan(m_Spec.DataFormat);
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkCreateImageView(vkDevice, &createInfo, nullptr, &m_ImageView);
	}

	VulkanFramebufferAttachment::~VulkanFramebufferAttachment()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyImageView(vkDevice, m_ImageView, nullptr);

		QK_DEBUG_CALL(m_ImageView = VK_NULL_HANDLE);
	}

	void VulkanFramebufferAttachment::SetData(void* data)
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyImageView(vkDevice, m_ImageView, nullptr);

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = static_cast<VkImage>(data);
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = InternalFormatToVulkan(m_Spec.DataFormat);
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(vkDevice, &createInfo, nullptr, &m_ImageView);
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec) : Framebuffer(spec)
	{
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		auto vkDevice = VulkanContext::GetCurrentDevice()->GetVkHandle();
		vkDestroyFramebuffer(vkDevice, m_Framebuffer, nullptr);

		QK_DEBUG_CALL(m_Framebuffer = VK_NULL_HANDLE);
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

		auto* attachments = static_cast<VkImageView*>(alloca(m_Spec.Attachments.size() * sizeof(VkImageView)));
		for (size_t i = 0; i < m_Spec.Attachments.size(); i++)
		{
			attachments[i] = static_cast<VulkanFramebufferAttachment*>(m_Spec.Attachments[i].get())->GetImageView();
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = static_cast<VulkanRenderPass*>(m_Spec.RenderPass.get())->GetVkHandle();
		framebufferInfo.attachmentCount = m_Spec.Attachments.size();
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Spec.Width;
		framebufferInfo.height = m_Spec.Height;
		framebufferInfo.layers = 1;

		vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &m_Framebuffer);
	}
}

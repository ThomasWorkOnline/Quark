#include "qkpch.h"
#include "VulkanFramebuffer.h"

#include "VulkanEnums.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.h"

#include "Quark/Renderer/Renderer.h"

namespace Quark {

	VulkanFramebufferAttachment::VulkanFramebufferAttachment(VulkanDevice* device, const FramebufferAttachmentSpecification& spec, VkImage image)
		: FramebufferAttachment(spec), m_Device(device), m_Image(image), m_SwapChainTarget(image)
	{
		Invalidate();
	}

	VulkanFramebufferAttachment::~VulkanFramebufferAttachment()
	{
		if (!m_SwapChainTarget)
		{
			vkDestroyImage(m_Device->GetVkHandle(), m_Image, nullptr);
			vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);
		}

		vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);
	}

	void VulkanFramebufferAttachment::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	bool VulkanFramebufferAttachment::operator==(const FramebufferAttachment& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_ImageView == o->m_ImageView;

		return false;
	}

	void VulkanFramebufferAttachment::SetSwapChainImage(VkImage image)
	{
		m_Image = image;
	}

	void VulkanFramebufferAttachment::Invalidate()
	{
		QK_PROFILE_FUNCTION();

		VkFormat format = DataFormatToVulkan(m_Spec.DataFormat);

		if (!m_SwapChainTarget)
		{
			vkDestroyImage(m_Device->GetVkHandle(), m_Image, nullptr);
			vkFreeMemory(m_Device->GetVkHandle(), m_BufferMemory, nullptr);

			VkExtent3D imageExtent{};
			imageExtent.width = m_Spec.Width;
			imageExtent.height = m_Spec.Height;
			imageExtent.depth = 1;

			Utils::AllocateImage(m_Device, imageExtent, 1, 1, SampleCountToVulkan(m_Spec.Samples), format,
				GetVulkanImageUsageFlags(m_Spec.DataFormat),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Image, &m_BufferMemory);
		}

		// Image view allocation
		{
			vkDestroyImageView(m_Device->GetVkHandle(), m_ImageView, nullptr);

			VkImageViewCreateInfo info{};
			info.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image                       = m_Image;
			info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
			info.format                      = format;
			info.subresourceRange.aspectMask = GetVulkanAspectFlags(m_Spec.DataFormat);
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;

			vkCreateImageView(m_Device->GetVkHandle(), &info, nullptr, &m_ImageView);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	// VulkanFramebuffer
	//

	VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const FramebufferSpecification& spec)
		: Framebuffer(spec)
		, m_Device(device)
	{
		QK_CORE_ASSERT(m_Spec.Attachments.size(), "Attachment Count must be non zero");
		QK_CORE_ASSERT(m_Spec.RenderPass, "RenderPass must be a valid pointer to a render pass");

		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		vkDestroyFramebuffer(m_Device->GetVkHandle(), m_Framebuffer, nullptr);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		auto& capabilities = Renderer::GetCapabilities();
		if (width == 0 || height == 0 || width > capabilities.Framebuffer.MaxWidth || height > capabilities.Framebuffer.MaxHeight)
		{
			QK_CORE_WARN("Attempted to resize a framebuffer with invalid dimensions: {0}, {1}", width, height);
			return;
		}

		for (auto& attachment : m_Spec.Attachments)
		{
			attachment->Resize(width, height);
		}

		m_Spec.Width = width;
		m_Spec.Height = height;

		Invalidate();
	}

	bool VulkanFramebuffer::operator==(const Framebuffer& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_Framebuffer == o->m_Framebuffer;

		return false;
	}

	void VulkanFramebuffer::Invalidate()
	{
		AutoRelease<VkImageView> attachments = StackAlloc(m_Spec.Attachments.size() * sizeof(VkImageView));

		uint32_t attachmentIndex = 0;
		for (auto& attachment : m_Spec.Attachments)
		{
			attachments[attachmentIndex] = static_cast<VulkanFramebufferAttachment*>(attachment.get())->GetVkHandle();
			attachmentIndex++;
		}

		vkDestroyFramebuffer(m_Device->GetVkHandle(), m_Framebuffer, nullptr);

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = static_cast<const VulkanRenderPass*>(m_Spec.RenderPass)->GetVkHandle();
		framebufferInfo.attachmentCount = attachmentIndex;
		framebufferInfo.pAttachments    = attachments;
		framebufferInfo.width           = m_Spec.Width;
		framebufferInfo.height          = m_Spec.Height;
		framebufferInfo.layers          = 1;

		vkCreateFramebuffer(m_Device->GetVkHandle(), &framebufferInfo, nullptr, &m_Framebuffer);
	}
}

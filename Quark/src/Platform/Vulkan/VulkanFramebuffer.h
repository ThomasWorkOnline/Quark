#pragma once

#include "Quark/Renderer/Framebuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		VulkanFramebufferAttachment(void* data, const FramebufferAttachmentSpecification& spec);
		virtual ~VulkanFramebufferAttachment() override;

		virtual void SetData(void* data) override;

		VkImageView GetImageView() const { return VK_NULL_HANDLE; }
	};

	class VulkanFramebuffer final : public Framebuffer
	{
	public:
		VulkanFramebuffer(VulkanDevice* device, const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer() override;

		virtual void Attach() override {}
		virtual void Detach() override {}

		virtual void Resize(uint32_t width, uint32_t height) override;

		VkFramebuffer GetVkHandle() const { return m_Framebuffer; }

		virtual bool operator==(const Framebuffer& other) const override
		{
			return m_Framebuffer == reinterpret_cast<const VulkanFramebuffer&>(other).m_Framebuffer;
		}

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}

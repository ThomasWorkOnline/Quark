#pragma once

#include "Quark/Renderer/Framebuffer.h"
#include <vulkan/vulkan.hpp>

namespace Quark {

	class VulkanFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		VulkanFramebufferAttachment(void* image, const FramebufferAttachmentSpecification& spec);
		virtual ~VulkanFramebufferAttachment() override;

		virtual void SetData(void* data) override;

		VkImageView GetImageView() const { return m_ImageView; }

	private:
		VkImageView m_ImageView = VK_NULL_HANDLE;
	};

	class VulkanFramebuffer final : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer() override;

		virtual void Attach() override {}
		virtual void Detach() override {}

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void AttachColorTextureTarget(uint32_t target, uint32_t textureRendererID) override {}
		virtual void AttachColorAttachment(uint32_t textureSlot, uint32_t index = 0) override {}
		virtual void AttachDepthAttachment(uint32_t textureSlot) override {}

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override { return m_Spec.Height; }

		vk::Framebuffer GetVkHandle() const { return m_Framebuffer; }

		virtual bool operator==(const Framebuffer& other) const override
		{
			return m_Framebuffer == reinterpret_cast<const VulkanFramebuffer&>(other).m_Framebuffer;
		}

	private:
		void Invalidate();

	private:
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}

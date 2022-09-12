#pragma once

#include "Quark/Renderer/Framebuffer.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		VulkanFramebufferAttachment() = default;
		VulkanFramebufferAttachment(VulkanDevice* device, const FramebufferAttachmentSpecification& spec);
		VulkanFramebufferAttachment(VulkanDevice* device, VkImage image, VkFormat format);
		virtual ~VulkanFramebufferAttachment() final override;

		virtual void SetData(const void* data) final override;

		virtual bool operator==(const FramebufferAttachment& other) const final override;

		VkImageView GetVkHandle() const { return m_ImageView; }

	private:
		VulkanDevice* m_Device = nullptr;
		VkImageView m_ImageView = nullptr;
		VkFormat m_Format{};
	};

	class VulkanFramebuffer final : public Framebuffer
	{
	public:
		VulkanFramebuffer(VulkanDevice* device, const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer() final override;

		virtual void Resize(uint32_t width, uint32_t height) final override;

		virtual bool operator==(const Framebuffer& other) const final override;

		VkFramebuffer GetVkHandle() const { return m_Framebuffer; }

		// Non-Copyable
		VulkanFramebuffer(const VulkanFramebuffer&) = delete;
		VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;
		VkFramebuffer m_Framebuffer = nullptr;
	};
}

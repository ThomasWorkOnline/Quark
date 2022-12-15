#pragma once

#include "Quark/Renderer/Framebuffer.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		VulkanFramebufferAttachment(VulkanDevice* device, const FramebufferAttachmentSpecification& spec);
		VulkanFramebufferAttachment(VulkanDevice* device, VkImage image, const FramebufferAttachmentSpecification& spec);
		virtual ~VulkanFramebufferAttachment() final override;

		virtual void Resize(uint32_t width, uint32_t height) final override;

		virtual bool operator==(const FramebufferAttachment& other) const final override;

		VkImageView GetVkHandle() const { return m_ImageView; }

		void SetSwapChainImage(VkImage image);

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device = nullptr;
		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_BufferMemory = nullptr;
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

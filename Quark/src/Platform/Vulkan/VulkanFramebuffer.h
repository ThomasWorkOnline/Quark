#pragma once

#include "Quark/Renderer/Framebuffer.h"

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Quark {

	class VulkanFramebufferAttachment final : public FramebufferAttachment
	{
	public:
		VulkanFramebufferAttachment() = default;
		VulkanFramebufferAttachment(VulkanDevice* device, VkImage image, const FramebufferAttachmentSpecification& spec);
		virtual ~VulkanFramebufferAttachment() final override;

		virtual void Resize(uint32_t width, uint32_t height) final override;

		virtual bool operator==(const FramebufferAttachment& other) const final override;

		void SetImage(VkImage image);

		VkImageView GetVkHandle() const { return m_ImageView; }

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device = nullptr;
		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_BufferMemory = nullptr;

		bool m_IsFromSwapChain = false;
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

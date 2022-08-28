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
		virtual ~VulkanFramebufferAttachment() override;

		virtual void SetData(const void* data) override;

		VkImageView GetVkHandle() const { return m_ImageView; }

	private:
		VulkanDevice* m_Device = nullptr;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkFormat m_Format{};
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

		// Non-Copyable
		VulkanFramebuffer(const VulkanFramebuffer&) = delete;
		VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

	private:
		void Invalidate();

	private:
		VulkanDevice* m_Device;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}

#pragma once

#include "Quark/Renderer/Texture.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace Quark {

	class VulkanTexture2D final : public Texture2D
	{
	public:
		VulkanTexture2D(VulkanDevice* device, const Texture2DSpecification& spec);
		VulkanTexture2D(VulkanDevice* device, std::string_view filepath, const TextureRenderModes& renderModes = {});
		virtual ~VulkanTexture2D() final override;

		virtual void Attach(uint32_t textureSlot = 0) const final override {}
		virtual void Detach() const final override {}

		virtual void SetData(const void* data, size_t size) final override {}
		virtual void GenerateMipmaps() final override {}

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;

	private:
		VulkanDevice* m_Device;
		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
	};
}

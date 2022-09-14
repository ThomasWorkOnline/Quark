#pragma once

#include "Quark/Renderer/Texture.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanTexture2D final : public Texture2D
	{
	public:
		VulkanTexture2D(VulkanDevice* device, const Texture2DSpecification& spec);
		VulkanTexture2D(VulkanDevice* device, std::string_view filepath, const SamplerRenderModes& renderModes = {});
		virtual ~VulkanTexture2D() final override;

		virtual void SetData(const void* data, size_t size) final override;
		virtual void GenerateMipmaps() final override;

		virtual const void* GetHandle() const final override { return m_ImageView; }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;

		VkImageView GetVkHandle() const { return m_ImageView; }

	private:
		VulkanDevice* m_Device;
		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_BufferMemory = nullptr;
	};

	class VulkanTexture2DArray final : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(VulkanDevice* device, const Texture2DArraySpecification& spec);
		virtual ~VulkanTexture2DArray() final override = default;

		virtual void SetData(const void* data, size_t size, uint32_t layer) final override {}
		virtual void GenerateMipmaps() final override {}

		virtual const void* GetHandle() const final override { return nullptr; }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		VulkanTexture2DArray(const VulkanTexture2DArray&) = delete;
		VulkanTexture2DArray& operator=(const VulkanTexture2DArray&) = delete;

	private:
		VulkanDevice* m_Device;
	};
}

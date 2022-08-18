#pragma once

#include "Quark/Renderer/Texture.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanTexture2D final : public Texture2D
	{
	public:
		VulkanTexture2D(VulkanDevice* device, const Texture2DSpecification& spec);
		VulkanTexture2D(VulkanDevice* device, std::string_view filepath, const TextureFormatDescriptor& descriptor = {});
		virtual ~VulkanTexture2D() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual void SetData(const void* data, size_t size) override {}
		virtual void GenerateMipmaps() override {}

		virtual bool operator==(const Texture2D& other) const override
		{
			return false;
		}

		// Non-Copyable
		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;

	private:
		VulkanDevice* m_Device;
	};
}

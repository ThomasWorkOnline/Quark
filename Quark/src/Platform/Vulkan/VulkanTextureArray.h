#pragma once

#include "Quark/Renderer/Texture.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanTexture2DArray final : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(VulkanDevice* device, const Texture2DArraySpecification& spec);
		virtual ~VulkanTexture2DArray() final override = default;

		virtual void Attach(uint32_t textureSlot = 0) const final override {}
		virtual void Detach() const final override {}

		virtual void SetData(const void* data, size_t size, uint32_t layer) final override {}
		virtual void GenerateMipmaps() final override {}

		virtual bool operator==(const Texture& other) const final override
		{
			return false;
		}

		// Non-Copyable
		VulkanTexture2DArray(const VulkanTexture2DArray&) = delete;
		VulkanTexture2DArray& operator=(const VulkanTexture2DArray&) = delete;

	private:
		VulkanDevice* m_Device;
	};
}

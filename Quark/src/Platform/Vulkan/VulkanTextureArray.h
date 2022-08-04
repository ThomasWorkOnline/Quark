#pragma once

#include "Quark/Renderer/TextureArray.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Quark {

	class VulkanTexture2DArray final : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(VulkanDevice* device, const Texture2DArraySpecification& spec);
		virtual ~VulkanTexture2DArray() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual void SetData(const void* data, size_t size, uint32_t layer) override {}
		virtual void GenerateMipmaps() override {}

		virtual bool operator==(const Texture2DArray& other) const override
		{
			return false;
		}

	private:
		VulkanDevice* m_Device;
	};
}

#pragma once

#include "Quark/Renderer/TextureArray.h"

namespace Quark {

	class VulkanTexture2DArray final : public Texture2DArray
	{
	public:
		VulkanTexture2DArray(const Texture2DArraySpecification& spec) {}
		virtual ~VulkanTexture2DArray() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual void SetData(const void* data, size_t size, uint32_t layer) override {}
		virtual void GenerateMipmaps() override {}

		virtual uint32_t GetWidth() const override { return 0; }
		virtual uint32_t GetHeight() const override { return 0; }
		virtual uint32_t GetLayerCount() const override { return 0; }

		virtual bool operator==(const Texture2DArray& other) const override
		{
			return false;
		}
	};
}

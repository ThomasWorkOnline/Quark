#pragma once

#include "Quark/Renderer/Texture.h"

namespace Quark {

	class VulkanTexture2D final : public Texture2D
	{
	public:
		VulkanTexture2D(const Texture2DSpecification& spec) {}
		VulkanTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor = {}) {}
		virtual ~VulkanTexture2D() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual void SetData(const void* data, size_t size) override {}
		virtual void GenerateMipmaps() override {}

		virtual uint32_t GetWidth() const override { return 0; }
		virtual uint32_t GetHeight() const override { return 0; }

		virtual uint32_t GetRendererID() const override { return 0; }

		virtual bool operator==(const Texture2D& other) const override
		{
			return false;
		}
	};
}

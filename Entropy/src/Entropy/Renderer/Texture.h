#pragma once

#include "../Core/Core.h"

#include "TextureFormats.h"
#include "Ressource.h"

namespace Entropy {

	struct TextureSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		TextureDataFormat TextureFormat = TextureDataFormat::Default;
		TextureFilteringFormat FilteringFormat = TextureFilteringFormat::Default;
		TextureTilingFormat TilingFormat = TextureTilingFormat::Default;
	};

	class Texture : public Ressource
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& spec);
		static Ref<Texture2D> Create(const std::string& filepath);
	};
}

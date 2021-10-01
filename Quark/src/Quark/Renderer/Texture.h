#pragma once

#include "../Core/Core.h"

#include "TextureFormats.h"

#include "DeferredObject.h"

namespace Quark {

	struct TextureSpecification
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		TextureDataFormat DataFormat = TextureDataFormat::None;
		TextureRenderModes RenderModes;
	};

	class Texture : public DeferredObject
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& spec);
		static Ref<Texture2D> Create(const std::string& filepath, const TextureRenderModes& modes = {});
	};
}

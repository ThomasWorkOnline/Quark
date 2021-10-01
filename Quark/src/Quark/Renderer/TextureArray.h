#pragma once

#include "../Core/Core.h"

#include "DeferredObject.h"

#include "TextureFormats.h"

namespace Quark {

	struct TextureArraySpecification
	{
		uint32_t Width = 0, Height = 0, Layers = 0;
		uint32_t Samples = 1;

		TextureDataFormat DataFormat = TextureDataFormat::None;
		TextureRenderModes RenderModes;
	};

	class TextureArray : public DeferredObject
	{
	public:
		virtual ~TextureArray() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetLayerCount() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const TextureArray& other) const = 0;

		virtual void SetData(void* data, uint32_t size, uint32_t layer) = 0;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;
	};

	class Texture2DArray : public TextureArray
	{
	public:
		static Ref<Texture2DArray> Create(const TextureArraySpecification& spec);
	};
}

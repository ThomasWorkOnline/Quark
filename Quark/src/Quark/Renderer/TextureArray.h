#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "TextureFormats.h"

namespace Quark {

	struct Texture2DArraySpecification
	{
		uint32_t              Width = 0, Height = 0, Layers = 0;
		uint32_t              Samples = 1;
		uint8_t               Levels  = 1;

		ColorDataFormat       DataFormat{};
		TextureRenderModes    RenderModes;
	};

	class Texture2DArray
	{
	public:
		virtual ~Texture2DArray() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(const void* data, size_t size, uint32_t layer) = 0;
		virtual void GenerateMipmaps() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetLayerCount() const = 0;

		virtual bool operator==(const Texture2DArray& other) const = 0;

		static Ref<Texture2DArray> Create(const Texture2DArraySpecification& spec);
	};
}

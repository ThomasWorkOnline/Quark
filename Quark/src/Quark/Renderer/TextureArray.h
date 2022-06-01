#pragma once

#include "Quark/Core/Core.h"
#include "TextureFormats.h"
#include "RenderCommand.h"

namespace Quark {

	struct TextureArraySpecification
	{
		uint32_t              Width = 0, Height = 0, Layers = 0;
		uint32_t              Samples = 1;
		uint8_t               Levels  = 1;

		TextureDataFormat     DataFormat{};
		TextureInternalFormat InternalFormat{};
		TextureRenderModes    RenderModes;
	};

	class TextureArray
	{
	public:
		virtual ~TextureArray() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(const void* data, size_t size, uint32_t layer) = 0;
		virtual void GenerateMipmaps() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetLayerCount() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const TextureArray& other) const = 0;

		static const TextureHardwareConstraints& GetConstraints()
		{
			return RenderCommand::GetHardwareConstraints().TextureConstraints;
		}
	};

	class Texture2DArray : public TextureArray
	{
	public:
		static Ref<Texture2DArray> Create(const TextureArraySpecification& spec);
	};
}

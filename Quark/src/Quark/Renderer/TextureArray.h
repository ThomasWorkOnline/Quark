#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "TextureFormats.h"

namespace Quark {

	struct Texture2DArraySpecification
	{
		uint32_t              Width, Height, Layers;
		uint32_t              Samples = 1;
		uint8_t               Levels  = 1;

		ColorDataFormat       DataFormat;
		TextureRenderModes    RenderModes;
	};

	class Texture2DArray
	{
	public:
		Texture2DArray(const Texture2DArraySpecification& spec)
			: m_Spec(spec) {}

		virtual ~Texture2DArray() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(const void* data, size_t size, uint32_t layer) = 0;
		virtual void GenerateMipmaps() = 0;

		virtual bool operator==(const Texture2DArray& other) const = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }
		uint32_t GetLayerCount() const { return m_Spec.Layers; }

		const Texture2DArraySpecification& GetSpecification() const { return m_Spec; }

		static Scope<Texture2DArray> Create(const Texture2DArraySpecification& spec);

	protected:
		Texture2DArraySpecification m_Spec{};
	};
}

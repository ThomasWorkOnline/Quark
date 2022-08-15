#pragma once

#include "Quark/Core/Core.h"
#include "ColorFormats.h"
#include "TextureFormats.h"

namespace Quark {

	struct Texture2DSpecification
	{
		uint32_t              Width = 0, Height = 0;
		uint32_t              Samples = 1;

		ColorDataFormat       DataFormat{};
		TextureRenderModes    RenderModes;
	};

	struct TextureFormatDescriptor
	{
		bool SRGB = false;
		TextureRenderModes RenderModes;
	};

	class Texture2D
	{
	public:
		Texture2D() = default;
		Texture2D(const Texture2DSpecification& spec)
			: m_Spec(spec) {}

		virtual ~Texture2D() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual void SetData(const void* data, size_t size) = 0;
		virtual void GenerateMipmaps() = 0;

		virtual bool operator==(const Texture2D& other) const = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }

		const Texture2DSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Texture2D> Create(const Texture2DSpecification& spec);
		static Scope<Texture2D> Create(std::string_view filepath, const TextureFormatDescriptor& descriptor = {});

	protected:
		Texture2DSpecification m_Spec;
	};
}

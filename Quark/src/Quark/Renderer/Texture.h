#pragma once

#include "Quark/Core/Core.h"

#include "ColorFormats.h"
#include "TextureFormats.h"

namespace Quark {

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	struct Texture2DSpecification
	{
		uint32_t              Width = 0, Height = 0;
		uint32_t              Samples = 1;
		uint32_t              Levels = 0;

		ColorDataFormat       DataFormat{};
		TextureRenderModes    RenderModes{};
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(const Texture2DSpecification& spec)
			: m_Spec(spec) {}

		virtual void SetData(const void* data, size_t size) = 0;
		virtual void GenerateMipmaps() = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }

		const Texture2DSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Texture2D> Create(const Texture2DSpecification& spec);
		static Scope<Texture2D> Create(std::string_view filepath, const TextureRenderModes& renderModes = {});

	protected:
		Texture2DSpecification m_Spec{};
	};

	struct Texture2DArraySpecification
	{
		uint32_t              Width = 0, Height = 0, Layers = 0;
		uint32_t              Samples = 1;
		uint32_t              Levels = 0;

		ColorDataFormat       DataFormat{};
		TextureRenderModes    RenderModes{};
	};

	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(const Texture2DArraySpecification& spec)
			: m_Spec(spec) {}

		virtual void SetData(const void* data, size_t size, uint32_t layer) = 0;
		virtual void GenerateMipmaps() = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }
		uint32_t GetLayerCount() const { return m_Spec.Layers; }

		const Texture2DArraySpecification& GetSpecification() const { return m_Spec; }

		static Scope<Texture2DArray> Create(const Texture2DArraySpecification& spec);

	protected:
		Texture2DArraySpecification m_Spec{};
	};
}

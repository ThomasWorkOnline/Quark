#pragma once

#include "Quark/Core/Core.h"
#include "Formats.h"

namespace Quark {

	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual const void* GetHandle() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	struct Texture2DSpecification
	{
		uint32_t              Width = 0, Height = 0;
		uint32_t              Levels = 0;
		uint32_t              Samples = 1;

		ColorFormat           DataFormat{};
		SamplerRenderModes    RenderModes{};
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		Texture2D(const Texture2DSpecification& spec)
			: m_Spec(spec) {}

		virtual ~Texture2D() = default;

		virtual void SetData(const void* data, size_t size) = 0;
		virtual void GenerateMipmaps() = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }

		const Texture2DSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Texture2D> Create(const Texture2DSpecification& spec);
		static Scope<Texture2D> Create(std::string_view filepath, const SamplerRenderModes& renderModes = {});

	protected:
		Texture2DSpecification m_Spec{};
	};

	struct Texture2DArraySpecification
	{
		uint32_t              Width = 0, Height = 0, Layers = 0;
		uint32_t              Samples = 1;
		uint32_t              Levels = 0;

		ColorFormat       DataFormat{};
		SamplerRenderModes    RenderModes{};
	};

	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(const Texture2DArraySpecification& spec)
			: m_Spec(spec) {}

		virtual ~Texture2DArray() = default;

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

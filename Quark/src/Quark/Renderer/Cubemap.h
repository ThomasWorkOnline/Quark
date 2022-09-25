#pragma once

#include "Quark/Core/Core.h"

#include "Formats.h"
#include "Texture.h"

namespace Quark {

	struct CubemapSpecification
	{
		uint32_t    Width = 0, Height = 0;
		ColorFormat DataFormat = ColorFormat::RGB16f;
	};

	class Cubemap : public Texture
	{
	public:
		Cubemap(const CubemapSpecification& spec)
			: m_Spec(spec) {}

		virtual ~Cubemap() = default;

		virtual void SetData(uint32_t index, const void* data, size_t size) = 0;

		uint32_t GetWidth() const { return m_Spec.Width; }
		uint32_t GetHeight() const { return m_Spec.Height; }

		const CubemapSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Cubemap> Create(const CubemapSpecification& spec);

	protected:
		CubemapSpecification m_Spec{};
	};
}

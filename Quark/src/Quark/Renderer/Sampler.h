#pragma once

#include "Quark/Core/Core.h"

namespace Quark {

	struct Sampler2DSpecification
	{
		uint32_t SamplerCount = 0;
		uint32_t Binding = 0;
	};

	class Sampler2D
	{
	public:
		Sampler2D(const Sampler2DSpecification& spec);
		virtual ~Sampler2D() = default;

		uint32_t GetBinding() const { return m_Spec.Binding; }

		const Sampler2DSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Sampler2D> Create(const Sampler2DSpecification& spec);

	protected:
		Sampler2DSpecification m_Spec{};
	};
}

#pragma once

#include "Quark/Core/Core.h"
#include "Formats.h"

namespace Quark {

	struct SamplerSpecification
	{
		SamplerRenderModes RenderModes{};
	};

	class Sampler
	{
	public:
		Sampler(const SamplerSpecification& spec);
		virtual ~Sampler() = default;

		virtual bool operator==(const Sampler& other) const = 0;

		const SamplerSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Sampler> Create(const SamplerSpecification& spec);

	protected:
		SamplerSpecification m_Spec{};
	};
}

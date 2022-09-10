#pragma once

#include "Quark/Core/Core.h"
#include "TextureFormats.h"

namespace Quark {

	struct SamplerSpecification
	{
		uint32_t Binding = 0;
		TextureRenderModes RenderModes{};
	};

	class Sampler
	{
	public:
		Sampler(const SamplerSpecification& spec);
		virtual ~Sampler() = default;

		virtual bool operator==(const Sampler& other) const = 0;

		uint32_t GetBinding() const { return m_Spec.Binding; }

		const SamplerSpecification& GetSpecification() const { return m_Spec; }

		static Scope<Sampler> Create(const SamplerSpecification& spec);

	protected:
		SamplerSpecification m_Spec{};
	};
}

#include "qkpch.h"
#include "Sampler.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Sampler::Sampler(const SamplerSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<Sampler> Sampler::Create(const SamplerSpecification& spec)
	{
		return s_GraphicsAPI->CreateSampler(spec);
	}
}

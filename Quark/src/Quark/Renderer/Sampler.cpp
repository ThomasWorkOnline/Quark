#include "qkpch.h"
#include "Sampler.h"

#include "GraphicsAPI.h"

namespace Quark {

	Sampler2D::Sampler2D(const Sampler2DSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<Sampler2D> Sampler2D::Create(const Sampler2DSpecification& spec)
	{
		return GraphicsAPI::Instance->CreateSampler2D(spec);
	}
}

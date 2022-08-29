#include "qkpch.h"
#include "Sampler.h"

#include "GraphicsAPI.cpp"

namespace Quark {

	Sampler2D::Sampler2D(const Sampler2DSpecification& spec)
		: m_Spec(spec)
	{
	}

	Scope<Sampler2D> Sampler2D::Create(const Sampler2DSpecification& spec)
	{
		return s_GraphicsAPI->CreateSampler2D(spec);
	}
}

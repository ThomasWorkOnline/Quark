#pragma once

#include "Quark/Renderer/Sampler.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLSampler : public Sampler
	{
	public:
		OpenGLSampler(const SamplerSpecification& spec);
		virtual ~OpenGLSampler() final override;

		virtual bool operator==(const Sampler& other) const final override;

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID;
	};
}

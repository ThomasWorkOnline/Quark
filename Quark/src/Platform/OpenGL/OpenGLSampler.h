#pragma once

#include "Quark/Renderer/Sampler.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLSampler2D : public Sampler2D
	{
	public:
		OpenGLSampler2D(const Sampler2DSpecification& spec);
		virtual ~OpenGLSampler2D() final override;

		virtual bool operator==(const Sampler2D& other) const final override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID;
	};
}

#include "qkpch.h"
#include "OpenGLSampler.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLSampler2D::OpenGLSampler2D(const Sampler2DSpecification& spec)
		: Sampler2D(spec)
	{
		glGenSamplers(1, &m_RendererID);

		glSamplerParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		QK_CORE_ASSERT(glIsSampler(m_RendererID), "Sampler is incomplete!");
	}

	OpenGLSampler2D::~OpenGLSampler2D()
	{
		glDeleteSamplers(1, &m_RendererID);
	}

	bool OpenGLSampler2D::operator==(const Sampler2D& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}

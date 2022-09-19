#include "qkpch.h"
#include "OpenGLSampler.h"

#include "OpenGLEnums.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLSampler::OpenGLSampler(const SamplerSpecification& spec)
		: Sampler(spec)
	{
		QK_CORE_ASSERT(!IsFormatUsingMips(m_Spec.RenderModes.MagFilteringMode),
			"The magnification mode may not be set to use mipmaps");

		glGenSamplers(1, &m_RendererID);
		glBindSampler(spec.Binding, m_RendererID);

		GLenum tilingMode = SamplerAddressModeToOpenGL(m_Spec.RenderModes.AddressMode);
		glSamplerParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, SamplerFilterModeToOpenGL(m_Spec.RenderModes.MinFilteringMode));
		glSamplerParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, SamplerFilterModeToOpenGL(m_Spec.RenderModes.MagFilteringMode));
		glSamplerParameteri(m_RendererID, GL_TEXTURE_WRAP_S, tilingMode);
		glSamplerParameteri(m_RendererID, GL_TEXTURE_WRAP_T, tilingMode);

		QK_CORE_ASSERT(glIsSampler(m_RendererID), "Sampler is incomplete!");
	}

	OpenGLSampler::~OpenGLSampler()
	{
		glDeleteSamplers(1, &m_RendererID);
	}

	bool OpenGLSampler::operator==(const Sampler& other) const
	{
		if (auto* o = dynamic_cast<decltype(this)>(&other))
			return m_RendererID == o->m_RendererID;

		return false;
	}
}

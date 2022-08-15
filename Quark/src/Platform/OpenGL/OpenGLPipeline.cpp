#include "qkpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec) : Pipeline(spec),
		m_UniformBuffer(spec.CameraUniformBufferSize, 0)
	{
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}
}

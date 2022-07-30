#include "qkpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec) : Pipeline(spec)
	{
		m_UniformBuffer = UniformBuffer::Create(m_Spec.CameraUniformBufferSize, 0);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}

	void OpenGLPipeline::Bind(const Ref<CommandBuffer>& commandBuffer)
	{
		m_Spec.Shader->Attach();
	}

	void OpenGLPipeline::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, viewportWidth, viewportHeight);
	}
}

#include "qkpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: m_Spec(spec)
	{
		m_UniformBuffer = UniformBuffer::Create(m_Spec.CameraUniformBufferSize, 0);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}

	void OpenGLPipeline::BeginFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLPipeline::EndFrame()
	{
	}

	void OpenGLPipeline::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		// Render passes do not exist in OpenGL
	}

	void OpenGLPipeline::EndRenderPass()
	{
	}

	void OpenGLPipeline::Submit()
	{
	}

	void OpenGLPipeline::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, viewportWidth, viewportHeight);
	}
}

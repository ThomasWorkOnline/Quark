#include "qkpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Quark {

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec) : Pipeline(spec)
	{
		m_CommandBuffer = CommandBuffer::Create();
		m_UniformBuffer = UniformBuffer::Create(m_Spec.CameraUniformBufferSize, 0);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
	}

	void OpenGLPipeline::BeginFrame()
	{
		m_Spec.Shader->Attach();
	}

	void OpenGLPipeline::EndFrame()
	{
	}

	void OpenGLPipeline::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		if (renderPass->GetSpecification().Clears)
		{
			glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// Render passes do not exist in OpenGL
	}

	void OpenGLPipeline::EndRenderPass()
	{
	}

	void OpenGLPipeline::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, viewportWidth, viewportHeight);
	}
}

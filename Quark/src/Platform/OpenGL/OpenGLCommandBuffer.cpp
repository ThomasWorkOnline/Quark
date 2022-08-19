#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "OpenGLBuffer.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace Quark {

	namespace Utils {

		GLenum PrimitiveTopologyToOpenGL(PrimitiveTopology topology)
		{
			switch (topology)
			{
				case PointList:     return GL_POINTS;
				case LineList:      return GL_LINES;
				case LineStrip:     return GL_LINE_STRIP;
				case TriangleList:  return GL_TRIANGLES;
				case TriangleStrip: return GL_TRIANGLE_STRIP;
				case TriangleFan:   return GL_TRIANGLE_FAN;

				QK_ASSERT_NO_DEFAULT("Unknown primitive topology");
			}

			return GL_NONE;
		}
	}

	void OpenGLCommandBuffer::BindPipeline(Pipeline* pipeline)
	{
		GLuint rendererID = reinterpret_cast<OpenGLShader*>(pipeline->GetSpecification().Shader)->GetRendererID();
		glUseProgram(rendererID);
	}

	void OpenGLCommandBuffer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, viewportWidth, viewportHeight);
	}

	void OpenGLCommandBuffer::SetPrimitiveTopology(PrimitiveTopology topology)
	{
		m_PrimitiveTopology = Utils::PrimitiveTopologyToOpenGL(topology);
	}

	void OpenGLCommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer)
	{
		framebuffer ? framebuffer->Attach() : glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (renderPass->GetSpecification().ClearBuffers)
		{
			glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	void OpenGLCommandBuffer::EndRenderPass()
	{
	}

	void OpenGLCommandBuffer::Draw(uint32_t vertexCount, uint32_t vertexOffset)
	{
		glDrawArrays(m_PrimitiveTopology, vertexOffset, vertexCount);
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(m_PrimitiveTopology, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLCommandBuffer::DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
	{
		glDrawArraysInstanced(m_PrimitiveTopology, vertexOffset, vertexCount, instanceCount);
	}

	void OpenGLCommandBuffer::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount)
	{
		glDrawElementsInstanced(m_PrimitiveTopology, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLCommandBuffer::BindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		GLuint vaoRendererID = static_cast<OpenGLVertexBuffer*>(vertexBuffer)->GetVAORendererID();
		glBindVertexArray(vaoRendererID);
	}

	void OpenGLCommandBuffer::BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		GLuint rendererID = static_cast<OpenGLIndexBuffer*>(indexBuffer)->GetRendererID();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	}
}

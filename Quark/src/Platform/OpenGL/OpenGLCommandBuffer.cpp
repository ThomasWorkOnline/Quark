#include "qkpch.h"
#include "OpenGLCommandBuffer.h"

#include "Quark/Renderer/Renderer.h"

#include "OpenGLBuffer.h"
#include "OpenGLEnums.h"
#include "OpenGLFont.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLPipeline.h"
#include "OpenGLSampler.h"
#include "OpenGLTexture.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Quark {

	static GLuint s_PushBufferRendererID = 0;
	static const BufferLayout* s_BoundLayout = nullptr;
	static const OpenGLPipeline* s_BoundPipeline = nullptr;

	OpenGLCommandBuffer::OpenGLCommandBuffer()
	{
		GLint maxBindings;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);

		glGenBuffers(1, &s_PushBufferRendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, s_PushBufferRendererID);
		glBindBufferBase(GL_UNIFORM_BUFFER, maxBindings - 1, s_PushBufferRendererID);
		glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_DYNAMIC_DRAW);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	OpenGLCommandBuffer::~OpenGLCommandBuffer()
	{
		glDeleteBuffers(1, &s_PushBufferRendererID);
	}

	void OpenGLCommandBuffer::SetCullMode(RenderCullMode mode)
	{
		glCullFace(CullModeToOpenGL(mode));
	}

	void OpenGLCommandBuffer::SetDepthFunction(DepthCompareFunction func)
	{
		glDepthFunc(DepthCompareFunctionToOpenGL(func));
	}

	void OpenGLCommandBuffer::BindPipeline(const Pipeline* pipeline)
	{
		s_BoundPipeline = static_cast<const OpenGLPipeline*>(pipeline);
		s_BoundPipeline->Bind();
	}

	void OpenGLCommandBuffer::BindDescriptorSets(const Pipeline* pipeline, uint32_t frameIndex)
	{
	}

	void OpenGLCommandBuffer::PushConstant(const Pipeline* pipeline, ShaderStage stage, const void* data, size_t size)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, s_PushBufferRendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);

		QK_DEBUG_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

	void OpenGLCommandBuffer::SetViewport(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glViewport(0, 0, (GLsizei)viewportWidth, (GLsizei)viewportHeight);
	}

	void OpenGLCommandBuffer::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void OpenGLCommandBuffer::BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer)
	{
		static_cast<const OpenGLFramebuffer*>(framebuffer)->Bind();

		m_CurrentRenderPass = renderPass;
		if (m_CurrentRenderPass->GetSpecification().ClearBuffers)
		{
			auto& color = m_CurrentRenderPass->GetSpecification().ClearColor;

			glClearColor(color.r, color.g, color.b, color.a);
			GLenum bufferBits = GL_COLOR_BUFFER_BIT;

			if (renderPass->GetSpecification().DepthAttachmentFormat != ColorFormat::None)
			{
				glClearDepthf(renderPass->GetSpecification().ClearDepth);
				bufferBits |= GL_DEPTH_BUFFER_BIT;
			}

			glClear(bufferBits);
		}
	}

	void OpenGLCommandBuffer::EndRenderPass()
	{
		m_CurrentRenderPass = nullptr;
	}

	void OpenGLCommandBuffer::Draw(uint32_t vertexCount, uint32_t vertexOffset)
	{
		glDrawArrays(s_BoundPipeline->GetPrimitiveTopologyState(), vertexOffset, vertexCount);
	}

	void OpenGLCommandBuffer::DrawIndexed(uint32_t indexCount)
	{
		glDrawElements(s_BoundPipeline->GetPrimitiveTopologyState(), indexCount, GL_UNSIGNED_INT, NULL);
	}

	void OpenGLCommandBuffer::DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount)
	{
		glDrawArraysInstanced(s_BoundPipeline->GetPrimitiveTopologyState(), vertexOffset, vertexCount, instanceCount);
	}

	void OpenGLCommandBuffer::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount)
	{
		glDrawElementsInstanced(s_BoundPipeline->GetPrimitiveTopologyState(), indexCount, GL_UNSIGNED_INT, NULL, instanceCount);
	}

	void OpenGLCommandBuffer::BindVertexBuffer(const VertexBuffer* vertexBuffer)
	{
		QK_CORE_ASSERT(vertexBuffer->GetLayout() == s_BoundPipeline->GetLayout(), "Buffer layout does not match the currently bound pipeline layout");

		auto* glVertexBuffer = static_cast<const OpenGLVertexBuffer*>(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer->GetRendererID());

		if (!s_BoundLayout || vertexBuffer->GetLayout() != *s_BoundLayout)
		{
			s_BoundPipeline->BindVertexAttrib();
			s_BoundLayout = std::addressof(s_BoundPipeline->GetLayout());
		}
	}

	void OpenGLCommandBuffer::BindIndexBuffer(const IndexBuffer* indexBuffer)
	{
		auto* glIndexBuffer = static_cast<const OpenGLIndexBuffer*>(indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer->GetRendererID());
	}

	void OpenGLCommandBuffer::BindUniformBuffer(const Pipeline* pipeline, const UniformBuffer* uniformBuffer, uint32_t frameIndex, uint32_t binding)
	{
		auto* glUniformBuffer = static_cast<const OpenGLUniformBuffer*>(uniformBuffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, glUniformBuffer->GetRendererID());
	}

	void OpenGLCommandBuffer::BindTexture(const Pipeline* pipeline, const Texture* texture, const Sampler* sampler, uint32_t frameIndex, uint32_t binding, uint32_t samplerIndex)
	{
		QK_CORE_ASSERT(samplerIndex < Renderer::GetCapabilities().Sampler.MaxTextureUnits,
			"Sampler index out of range: max writable index is: {0}",
			Renderer::GetCapabilities().Sampler.MaxTextureUnits - 1);

		auto* glTexture = static_cast<const OpenGLTexture*>(texture->GetHandle());
		glActiveTexture(GL_TEXTURE0 + samplerIndex);
		glBindTexture(glTexture->GetTarget(), glTexture->GetRendererID());

		auto* glSampler = static_cast<const OpenGLSampler*>(sampler);
		glBindSampler(samplerIndex, glSampler->GetRendererID());
	}

	bool OpenGLCommandBuffer::IsInsideRenderPass() const
	{
		return m_CurrentRenderPass;
	}

	bool OpenGLCommandBuffer::operator==(const CommandBuffer& other) const
	{
		// Returns true only if other is an OpenGLCommandBuffer
		return dynamic_cast<decltype(this)>(&other);
	}
}

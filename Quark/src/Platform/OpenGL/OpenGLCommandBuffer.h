#pragma once

#include "Quark/Renderer/CommandBuffer.h"
#include "OpenGLPipeline.h"

namespace Quark {

	class OpenGLCommandBuffer : public CommandBuffer
	{
	public:
		OpenGLCommandBuffer() = default;
		virtual ~OpenGLCommandBuffer() = default;

		virtual void Begin() final override {}
		virtual void End() final override {}

		virtual void BindPipeline(Pipeline* pipeline) final override;
		virtual void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual void BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer) final override;
		virtual void EndRenderPass() final override;

		virtual void Reset() final override {}
		virtual void Draw(uint32_t vertexOffset, uint32_t vertexCount) final override;
		virtual void DrawIndexed(uint32_t indexCount) final override;
		virtual void DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount) final override;

		virtual void DrawLines(uint32_t vertexCount) final override;

		virtual void BindVertexBuffer(VertexBuffer* vertexBuffer) final override;
		virtual void BindIndexBuffer(IndexBuffer* indexBuffer) final override;

		virtual bool operator==(const CommandBuffer& other) const final override
		{
			return true;
		}

		// Non-Copyable
		OpenGLCommandBuffer(const OpenGLCommandBuffer&) = delete;
		OpenGLCommandBuffer& operator=(const OpenGLCommandBuffer&) = delete;

	private:
		OpenGLPipeline* m_BoundPipeline = nullptr;
	};
}

#pragma once

#include "Quark/Core/Core.h"

#include "Buffer.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"

namespace Quark {

	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void BindPipeline(Pipeline* pipeline) = 0;

		virtual void BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer) = 0;
		virtual void EndRenderPass() = 0;

		virtual void Reset() = 0;
		virtual void Draw(uint32_t vertexOffset, uint32_t vertexCount) = 0;
		virtual void DrawIndexed(uint32_t indexCount) = 0;
		virtual void DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount) = 0;

		virtual void DrawLines(uint32_t vertexCount) = 0;

		virtual void BindVertexBuffer(VertexBuffer* vertexBuffer) = 0;
		virtual void BindIndexBuffer(IndexBuffer* indexBuffer) = 0;

		virtual bool operator==(const CommandBuffer& other) const = 0;

		static CommandBuffer* Create();
	};
}

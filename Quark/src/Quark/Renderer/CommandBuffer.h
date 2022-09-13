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
		virtual void Reset() = 0;

		virtual void SetCullFace(RenderCullMode mode) = 0;
		virtual void SetDepthFunction(RenderDepthFunction func) = 0;

		virtual void BindPipeline(const Pipeline* pipeline) = 0;
		virtual void PushConstant(const Pipeline* pipeline, ShaderStage stage, const void* data, size_t size) = 0;

		virtual void SetViewport(uint32_t viewportWidth, uint32_t viewportHeight) = 0;
		virtual void SetLineWidth(float width) = 0;

		virtual void BeginRenderPass(const RenderPass* renderPass, const Framebuffer* framebuffer) = 0;
		virtual void EndRenderPass() = 0;

		virtual void Draw(uint32_t vertexCount, uint32_t vertexOffset) = 0;
		virtual void DrawIndexed(uint32_t indexCount) = 0;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t vertexOffset, uint32_t instanceCount) = 0;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount) = 0;

		virtual void BindVertexBuffer(const VertexBuffer* vertexBuffer) = 0;
		virtual void BindIndexBuffer(const IndexBuffer* indexBuffer) = 0;

		virtual bool IsInsideRenderPass() const = 0;

		virtual bool operator==(const CommandBuffer& other) const = 0;

		static Scope<CommandBuffer> Create();
	};
}

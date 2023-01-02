#pragma once

#include "Quark/Core/Core.h"

#include "Buffer.h"
#include "Font.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Sampler.h"
#include "Texture.h"
#include "UniformBuffer.h"

namespace Quark {

	class CommandBuffer
	{
	public:
		CommandBuffer() = default;
		virtual ~CommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Reset() = 0;

		virtual void SetCullMode(RenderCullMode mode) = 0;
		virtual void SetDepthFunction(DepthCompareFunction func) = 0;

		virtual void BindPipeline(const Pipeline* pipeline) = 0;
		virtual void BindDescriptorSets(const Pipeline* pipeline, uint32_t frameIndex) = 0;
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

		virtual void BindUniformBuffer(const Pipeline* pipeline, const UniformBuffer* uniformBuffer, uint32_t frameIndex, uint32_t binding) = 0;
		virtual void BindTexture(const Pipeline* pipeline, const Texture* texture, const Sampler* sampler, uint32_t frameIndex, uint32_t binding, uint32_t samplerIndex = 0) = 0;

		virtual bool IsInsideRenderPass() const = 0;

		virtual bool operator==(const CommandBuffer& other) const = 0;

		static Scope<CommandBuffer> Create();
	};
}

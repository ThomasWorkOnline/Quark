#pragma once

#include "Quark/Renderer/CommandBuffer.h"

namespace Quark {

	class OpenGLCommandBuffer final : public CommandBuffer
	{
	public:
		virtual void Begin() override {}
		virtual void End() override {}

		virtual void BindPipeline(const Ref<Pipeline>& pipeline) override;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass, const Ref<Framebuffer>& framebuffer);
		virtual void EndRenderPass();

		virtual void Reset() override {}
		virtual void Draw(uint32_t vertexOffset, uint32_t vertexCount) override;
		virtual void DrawIndexed(uint32_t indexCount) override;
		virtual void DrawIndexedInstanced(uint32_t instanceCount, uint32_t indexCount) override;;

		virtual void DrawLines(uint32_t vertexCount) override;

		virtual void BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding) override;
		virtual void BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual bool operator==(const CommandBuffer& other) const override
		{
			return false;
		}
	};
}

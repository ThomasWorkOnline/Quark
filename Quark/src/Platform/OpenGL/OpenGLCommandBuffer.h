#pragma once

#include "Quark/Renderer/CommandBuffer.h"

namespace Quark {

	class OpenGLCommandBuffer final : public CommandBuffer
	{
	public:
		virtual void Begin() override {}
		virtual void End() override {}

		virtual void Reset() override {}
		virtual void DrawIndexed(uint32_t indexCount) override;

		virtual void BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding) override;
		virtual void BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual bool operator==(const CommandBuffer& other) const override
		{
			return false;
		}
	};
}

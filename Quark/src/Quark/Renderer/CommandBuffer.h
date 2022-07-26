#pragma once

#include "Quark/Core/Core.h"
#include "Buffer.h"

namespace Quark {

	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void Reset() = 0;
		virtual void DrawIndexed(uint32_t indexCount) = 0;

		virtual void BindVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, uint32_t binding) = 0;
		virtual void BindIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual bool operator==(const CommandBuffer& other) const = 0;

		static Ref<CommandBuffer> Create();
	};
}

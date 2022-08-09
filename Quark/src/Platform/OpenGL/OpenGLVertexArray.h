#pragma once

#include "Quark/Renderer/VertexArray.h"

namespace Quark {

	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		void Attach() const;
		void Detach() const;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		bool operator==(const VertexArray& other) const
		{
			return m_RendererID == ((OpenGLVertexArray&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
		BufferLayout m_Layout;
	};
}

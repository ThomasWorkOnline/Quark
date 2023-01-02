#pragma once

#include "Quark/Renderer/Buffer.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(size_t size);
		OpenGLVertexBuffer(const void* vertices, size_t size);
		virtual ~OpenGLVertexBuffer() final override;

		virtual void SetData(const void* data, size_t size, size_t offset) final override;

		virtual bool operator==(const VertexBuffer& other) const final override;

		// Non-Copyable
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = delete;

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID = 0;
		size_t m_Size;
	};

	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count, IndexType indexType);
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, IndexType indexType);
		virtual ~OpenGLIndexBuffer() final override;

		virtual void SetData(const uint32_t* data, uint32_t count, uint32_t firstIndex = 0) final override;
		virtual bool operator==(const IndexBuffer& other) const final override;

		// Non-Copyable
		OpenGLIndexBuffer(const OpenGLIndexBuffer&) = delete;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer&) = delete;

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID = 0;
	};
}

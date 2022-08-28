#pragma once

#include "Quark/Renderer/Buffer.h"

typedef unsigned int GLuint;

namespace Quark {

	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(size_t size);
		OpenGLVertexBuffer(const void* vertices, size_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void SetData(const void* data, size_t size, size_t offset) override;

		virtual bool operator==(const VertexBuffer& other) const override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

		// Non-Copyable
		OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = delete;

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID;
	};

	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count);
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void SetData(const uint32_t* data, uint32_t count, size_t offset) override;
		virtual uint32_t GetCount() const override { return m_Count; };

		virtual bool operator==(const IndexBuffer& other) const override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

		// Non-Copyable
		OpenGLIndexBuffer(const OpenGLIndexBuffer&) = delete;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer&) = delete;

		GLuint GetRendererID() const { return m_RendererID; }

	private:
		GLuint m_RendererID;
		uint32_t m_Count;
	};
}

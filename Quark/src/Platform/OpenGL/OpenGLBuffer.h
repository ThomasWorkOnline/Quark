#pragma once

#include "Quark/Renderer/Buffer.h"

namespace Quark {

	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(size_t size);
		OpenGLVertexBuffer(const void* vertices, size_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void Attach() const override;
		virtual void Detach() const override;

		virtual void SetData(const void* data, size_t size, size_t offset) override;
		virtual void SetLayout(const BufferLayout& layout) override;

		virtual bool operator==(const VertexBuffer& other) const override
		{
			return m_RendererID == ((OpenGLVertexBuffer&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID;
		uint32_t m_VAORendererID;
	};

	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count);
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Attach() const override;
		virtual void Detach() const override;

		virtual void SetData(const uint32_t* data, uint32_t count, size_t offset) override;
		virtual uint32_t GetCount() const override { return m_Count; };

		virtual bool operator==(const IndexBuffer& other) const override
		{
			return m_RendererID == ((OpenGLIndexBuffer&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}

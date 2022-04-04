#pragma once

#include "Quark/Renderer/Buffer.h"

namespace Quark {

    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(size_t size);
        OpenGLVertexBuffer(const void* vertices, size_t size);
        ~OpenGLVertexBuffer() override;

        void Attach() const override;
        void Detach() const override;

        const BufferLayout& GetLayout() const override { return m_layout; }
        void SetLayout(const BufferLayout& layout) override { m_layout = layout; }

        void SetData(const void* data, size_t size, size_t offset) override;

        uint32_t GetRendererID() const override { return m_RendererID; };

        bool operator==(const VertexBuffer& other) const override
        {
            return m_RendererID == ((OpenGLVertexBuffer&)other).m_RendererID;
        }

    private:
        uint32_t m_RendererID;
        BufferLayout m_layout;
    };

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t count);
        OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
        ~OpenGLIndexBuffer();

        void Attach() const override;
        void Detach() const override;

        void SetData(const uint32_t* data, uint32_t count, size_t offset) override;

        uint32_t GetCount() const override { return m_Count; };

        uint32_t GetRendererID() const override { return m_RendererID; };

        bool operator==(const IndexBuffer& other) const override
        {
            return m_RendererID == ((OpenGLIndexBuffer&)other).m_RendererID;
        }

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}

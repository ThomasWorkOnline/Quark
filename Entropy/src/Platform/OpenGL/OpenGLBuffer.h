#pragma once

#include "../../Entropy/Renderer/Buffer.h"

namespace Entropy {

    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(size_t size);
        OpenGLVertexBuffer(float* vertices, size_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void Attach() const override;
        virtual void Detach() const override;

        virtual const BufferLayout& GetLayout() const override { return m_layout; }
        virtual void SetLayout(const BufferLayout& layout) override { m_layout = layout; }

        virtual void SetData(const void* data, size_t size, size_t offset) override;

    private:
        uint32_t m_RendererID = 0;
        BufferLayout m_layout;
    };

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t count);
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Attach() const override;
        virtual void Detach() const override;

        virtual void SetData(uint32_t* data, uint32_t count, size_t offset) override;

        virtual uint32_t GetCount() const { return m_Count; };

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_Count;
    };
}

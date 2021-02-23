#pragma once

#include "../../Entropy/Renderer/VertexArray.h"

namespace Entropy {

    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Attach() const override;
        virtual void Detach() const override;

        virtual void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        virtual Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };
}

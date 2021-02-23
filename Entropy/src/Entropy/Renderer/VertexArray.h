#pragma once

#include "Buffer.h"

namespace Entropy {

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Attach() const = 0;
        virtual void Detach() const = 0;

        virtual void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual Ref<IndexBuffer> GetIndexBuffer() const = 0;

        static Ref<VertexArray> Create();
    };
}

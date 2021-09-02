#pragma once

#include "Buffer.h"
#include "Resource.h"
#include "RenderObject.h"

namespace Quark {

    class VertexArray : public Resource
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Attach() const = 0;
        virtual void Detach() const = 0;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        virtual uint32_t GetRendererID() const = 0;

        static Ref<VertexArray> Create();
    };
}

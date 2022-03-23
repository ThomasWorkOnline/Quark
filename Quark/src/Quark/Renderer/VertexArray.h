#pragma once

#include "Quark/Core/Core.h"

#include "Buffer.h"

namespace Quark {

    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Attach() const = 0;
        virtual void Detach() const = 0;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        virtual bool operator==(const VertexArray& other) const = 0;

        virtual uint32_t GetRendererID() const = 0;

        static Ref<VertexArray> Create();
    };
}

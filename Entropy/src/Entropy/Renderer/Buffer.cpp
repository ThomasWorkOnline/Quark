#include "Buffer.h"

#include "RenderingAPI.h"

// Include all supported API's buffers implementations
#include "../../Platform/OpenGL/OpenGLBuffer.h"

namespace Entropy {

    // Vertex buffer
    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, size_t size)
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(vertices, size);
        }

        NT_FATAL("Unknown Rendering API");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(size_t size)
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(size);
        }

        NT_FATAL("Unknown Rendering API");
        return nullptr;
    }



    // Index buffer
    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return CreateRef<OpenGLIndexBuffer>(indices, count);
        }

        NT_FATAL("Unknown Rendering API");
        return nullptr;
    }
}

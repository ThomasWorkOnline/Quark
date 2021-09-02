#include "Buffer.h"

#include "RenderingAPI.h"

// Include all supported API's buffers implementations
#include "../../Platform/OpenGL/OpenGLBuffer.h"

namespace Quark {

    Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size)
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return Resource::Create<OpenGLVertexBuffer>(vertices, size);
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(size_t size)
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return Resource::Create<OpenGLVertexBuffer>(size);
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return Resource::Create<OpenGLIndexBuffer>(indices, count);
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
    {
        switch (RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return Resource::Create<OpenGLIndexBuffer>(count);
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }
        return nullptr;
    }
}

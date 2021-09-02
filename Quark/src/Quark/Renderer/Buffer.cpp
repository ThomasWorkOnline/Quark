#include "Buffer.h"

#include "RenderingAPI.h"

// Include all supported API's buffers implementations
#include "../../Platform/OpenGL/OpenGLBuffer.h"

#include "../Core/Application.h"

namespace Quark {

    Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, size_t size)
    {
        Ref<VertexBuffer> ref;

        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            ref = CreateRef<OpenGLVertexBuffer>(vertices, size);
            break;
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }

        Application::Get().GetResourceManager().Hold(ref);
        return ref;
    }

    Ref<VertexBuffer> VertexBuffer::Create(size_t size)
    {
        Ref<VertexBuffer> ref;

        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            ref = CreateRef<OpenGLVertexBuffer>(size);
            break;
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }

        Application::Get().GetResourceManager().Hold(ref);
        return ref;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        Ref<IndexBuffer> ref;

        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            ref = CreateRef<OpenGLIndexBuffer>(indices, count);
            break;
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }

        Application::Get().GetResourceManager().Hold(ref);
        return ref;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
    {
        Ref<IndexBuffer> ref;

        switch (RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            ref = CreateRef<OpenGLIndexBuffer>(count);
            break;
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }

        Application::Get().GetResourceManager().Hold(ref);
        return ref;
    }
}

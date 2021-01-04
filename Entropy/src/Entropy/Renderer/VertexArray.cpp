#include "VertexArray.h"

#include "RenderingAPI.h"

// Include all supported API's vertex arrays implementations
#include "../../Platform/OpenGL/OpenGLVertexArray.h"

namespace Entropy {

    Ref<VertexArray> VertexArray::Create()
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return CreateRef<OpenGLVertexArray>();
        case RenderingAPI::API::None:
            NT_FATAL("Rendering API not supported");
            return nullptr;
        }

        NT_FATAL("Unknown Rendering API");
        return nullptr;
    }
}

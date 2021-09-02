#include "VertexArray.h"

#include "RenderingAPI.h"

// Include all supported API's vertex arrays implementations
#include "../../Platform/OpenGL/OpenGLVertexArray.h"

namespace Quark {

    Ref<VertexArray> VertexArray::Create()
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return Resource::Create<OpenGLVertexArray>();
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }
        return nullptr;
    }
}

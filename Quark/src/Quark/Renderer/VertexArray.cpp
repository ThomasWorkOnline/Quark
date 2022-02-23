#include "VertexArray.h"

#include "RenderingAPI.h"
#include "DeferredObjectDeleter.h"

// Include all supported API's vertex arrays implementations
#include "../../Platform/OpenGL/OpenGLVertexArray.h"

namespace Quark {

    Ref<VertexArray> VertexArray::Create()
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            return CreateRef<OpenGLVertexArray, DeferredObjectDeleter>();
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }
        return nullptr;
    }
}

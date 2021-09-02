#include "VertexArray.h"

#include "RenderingAPI.h"

// Include all supported API's vertex arrays implementations
#include "../../Platform/OpenGL/OpenGLVertexArray.h"

#include "../Core/Application.h"

namespace Quark {

    Ref<VertexArray> VertexArray::Create()
    {
        Ref<VertexArray> ref;

        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            ref = CreateRef<OpenGLVertexArray>();
            break;
        case RenderingAPI::API::None:
            QK_FATAL("Rendering API not supported");
        }

        Application::Get().GetResourceManager().Hold(ref);
        return ref;
    }
}

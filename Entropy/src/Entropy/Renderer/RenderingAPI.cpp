#include "RenderingAPI.h"

// Include all supported APIs here
#include "../../Platform/OpenGL/OpenGLRenderingAPI.h"

namespace Entropy {

    // Default api is OpenGL
    RenderingAPI::API RenderingAPI::s_API = RenderingAPI::API::OpenGL;

    Scope<RenderingAPI> RenderingAPI::Create()
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            NT_TRACE("Created the OpenGL rendering API");
            return CreateScope<OpenGLRenderingAPI>();
        case RenderingAPI::API::None:
            NT_FATAL("Rendering API not supported");
            return nullptr;
        }

        NT_FATAL("Unknown Rendering API");
        return nullptr;
    }
}

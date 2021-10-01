#include "RenderingAPI.h"

// Include all supported APIs here
#include "../../Platform/OpenGL/OpenGLRenderingAPI.h"

namespace Quark {

    // Default api is OpenGL
    RenderingAPI::API RenderingAPI::s_API = RenderingAPI::API::OpenGL;

    Scope<RenderingAPI> RenderingAPI::Create()
    {
        switch(RenderingAPI::GetAPI())
        {
        case RenderingAPI::API::OpenGL:
            QK_CORE_TRACE("Created the OpenGL rendering API");
            return CreateScope<OpenGLRenderingAPI>();
        }

        QK_FATAL("Unknown Rendering API");
        return nullptr;
    }

    const char* RenderingAPI::GetName()
    {
        switch (s_API)
        {
        case Quark::RenderingAPI::API::None:    return "None";
        case Quark::RenderingAPI::API::OpenGL:  return "OpenGL";
        }
    }
}

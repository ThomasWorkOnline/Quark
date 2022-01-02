#include "RenderingAPI.h"

// Include all supported APIs here
#include "../../Platform/OpenGL/OpenGLRenderingAPI.h"

namespace Quark {

    RenderingAPI::API RenderingAPI::s_API = RenderingAPI::API::None;
    Scope<RenderingAPI> RenderingApi = nullptr;

    const char* RenderingAPI::GetAPIName()
    {
        return RenderingApi->GetName();
    }

    void RenderingAPI::Create(API api)
    {
        s_API = api;
        switch (s_API)
        {
        case RenderingAPI::API::OpenGL:
            RenderingApi = CreateScope<OpenGLRenderingAPI>();
            break;
        case RenderingAPI::API::None:
            QK_FATAL("Unknown Rendering API");
            break;
        }
    }
}

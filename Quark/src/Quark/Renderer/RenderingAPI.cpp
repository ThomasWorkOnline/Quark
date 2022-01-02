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

    Scope<RenderingAPI> RenderingAPI::Create(API api)
    {
        s_API = api;
        switch (s_API)
        {
        case RenderingAPI::API::OpenGL:
            return CreateScope<OpenGLRenderingAPI>();
        }

        QK_FATAL("Unknown Rendering API");
        return nullptr;
    }
}

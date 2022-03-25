#include "RenderingAPI.h"

// Include all supported APIs here
#include "Platform/OpenGL/OpenGLRenderingAPI.h"

namespace Quark {

	RenderingAPI::API RenderingAPI::s_API = RenderingAPI::API::None;

	Scope<RenderingAPI> RenderingAPI::Create(API api)
	{
		s_API = api;
		switch (api)
		{
			case RenderingAPI::API::OpenGL:
				return CreateScope<OpenGLRenderingAPI>();
			default:
				QK_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}

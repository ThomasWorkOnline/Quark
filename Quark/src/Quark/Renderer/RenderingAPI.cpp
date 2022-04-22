#include "qkpch.h"
#include "RenderingAPI.h"

// Include all supported APIs here
#include "Platform/OpenGL/OpenGLRenderingAPI.h"

namespace Quark {

	RenderingAPI::API RenderingAPI::s_API = RenderingAPI::API::None;
	std::thread::id RenderingAPI::s_ThreadId;

	Scope<RenderingAPI> RenderingAPI::Create(API api)
	{
		s_API = api;
		s_ThreadId = std::this_thread::get_id();

		switch (api)
		{
			case RenderingAPI::API::OpenGL:
				return CreateScope<OpenGLRenderingAPI>();
			default:
				QK_CORE_FATAL("Rendering API not supported");
				return nullptr;
		}
	}
}

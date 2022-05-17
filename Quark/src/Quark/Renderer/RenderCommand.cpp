#include "qkpch.h"
#include "RenderCommand.h"

namespace Quark {

	std::thread::id RenderCommand::s_ThreadId;
	Scope<RenderingAPI> RenderCommand::s_RenderingApi = RenderingAPI::Create(RenderingAPI::API::OpenGL);
}

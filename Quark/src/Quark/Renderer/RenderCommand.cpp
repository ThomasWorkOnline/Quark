#include "qkpch.h"
#include "RenderCommand.h"

namespace Quark {

	std::thread::id RenderCommand::s_ThreadId;
	Scope<GraphicsAPI> RenderCommand::s_RenderingApi = GraphicsAPI::Create(GraphicsAPI::GetDefaultForPlatform());
}

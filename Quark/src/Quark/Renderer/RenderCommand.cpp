#include "qkpch.h"
#include "RenderCommand.h"

namespace Quark {

	std::thread::id RenderCommand::s_ThreadId;
	Scope<GraphicsAPI> RenderCommand::s_GraphicsAPI = GraphicsAPI::Create(GraphicsAPI::GetDefaultForPlatform());
}

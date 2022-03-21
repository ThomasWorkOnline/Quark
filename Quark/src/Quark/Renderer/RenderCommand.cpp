#include "RenderCommand.h"

namespace Quark {

	Scope<RenderingAPI> RenderCommand::s_RenderingApi = RenderingAPI::Create(RenderingAPI::API::OpenGL);
}

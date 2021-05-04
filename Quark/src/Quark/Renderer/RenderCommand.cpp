#include "RenderCommand.h"

namespace Quark {

	Scope<RenderingAPI> RenderCommand::s_RenderingAPI = RenderingAPI::Create();
}

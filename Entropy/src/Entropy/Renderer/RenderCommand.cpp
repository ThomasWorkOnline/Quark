#include "RenderCommand.h"

namespace Entropy {

	Scope<RenderingAPI> RenderCommand::s_RenderingAPI = RenderingAPI::Create();
}

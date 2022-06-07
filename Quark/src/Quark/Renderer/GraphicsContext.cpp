#include "qkpch.h"
#include "GraphicsContext.h"

#include "RenderCommand.h"

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle)
	{
		return RenderCommand::CreateGraphicsContext(windowHandle);
	}
}

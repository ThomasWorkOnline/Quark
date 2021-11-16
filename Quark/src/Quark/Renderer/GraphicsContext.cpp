#include "GraphicsContext.h"

#include "RenderingAPI.h"

// Include all supported API's graphics context implementations
#include "../../Platform/OpenGL/OpenGLGraphicsContext.h"

namespace Quark {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			QK_CORE_TRACE("Created OpenGL Graphics Context!");
			return CreateScope<OpenGLGraphicsContext>(window);
		}

		QK_FATAL("Unknown Rendering API");
		return nullptr;
	}
}

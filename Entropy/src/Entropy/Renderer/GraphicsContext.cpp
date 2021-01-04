#include "GraphicsContext.h"

#include "RenderingAPI.h"

// Include all supported API's graphics context implementations
#include "../../Platform/OpenGL/OpenGLGraphicsContext.h"

namespace Entropy {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			NT_TRACE("Created OpenGL Graphics Context!");
			return CreateScope<OpenGLGraphicsContext>(static_cast<GLFWwindow*>(window));
		case RenderingAPI::API::None:
			NT_FATAL("Could not create graphics context");
			return nullptr;
		}

		NT_FATAL("Unknown Rendering API");
		return nullptr;
	}
}

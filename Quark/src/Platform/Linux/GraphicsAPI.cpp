#include "qkpch.h"
#include "GraphicsAPI.h"

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"

namespace Quark {

	RHI GetDefaultRHIForPlatform()
	{
		return RHI::OpenGL;
	}

	Scope<GraphicsAPI> CreateDefaultRHIForPlatform()
	{
		return CreateScope<OpenGLGraphicsAPI>();
	}
}

#include "Framebuffer.h"

#include "RenderingAPI.h"

// Include all supported API's frame buffer implementations
#include "../../Platform/OpenGL/OpenGLFramebuffer.h"

#include "../Core/Application.h"

namespace Quark {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		Ref<Framebuffer> ref;

		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPI::API::OpenGL:
			ref = CreateRef<OpenGLFramebuffer>(spec);
			break;
		case RenderingAPI::API::None:
			QK_FATAL("Rendering API not supported");
		}

		Application::Get().GetResourceManager().Hold(ref);
		return ref;
	}
}

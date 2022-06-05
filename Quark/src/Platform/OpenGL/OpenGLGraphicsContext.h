#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLGLFWGraphicsContext final : public GraphicsContext
	{
	public:
		OpenGLGLFWGraphicsContext(void* windowHandle);
		virtual ~OpenGLGLFWGraphicsContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

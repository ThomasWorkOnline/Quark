#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLContext final : public GraphicsContext
	{
	public:
		OpenGLContext(void* windowHandle);
		virtual ~OpenGLContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

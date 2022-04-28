#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLGLFWGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGLFWGraphicsContext(void* windowHandle);
		~OpenGLGLFWGraphicsContext() override;

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

#pragma once

#include "../../Quark/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Quark {

	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}

#pragma once

#include "OpenGLContextBase.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLContext final : public OpenGLContextBase
	{
	public:
		OpenGLContext(void* windowHandle);
		virtual ~OpenGLContext() override;

		virtual void Init() override;

		virtual void SwapBuffers() override;
		virtual void SetSwapInterval(int interval) override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

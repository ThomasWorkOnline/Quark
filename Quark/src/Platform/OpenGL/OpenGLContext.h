#pragma once

#include "Quark/Renderer/GraphicsContext.h"
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

		// Non-Copyable
		OpenGLContext(const OpenGLContext&) = delete;
		OpenGLContext& operator=(const OpenGLContext&) = delete;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

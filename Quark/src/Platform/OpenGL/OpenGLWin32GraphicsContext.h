#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef HWND  HWND;
typedef HGLRC HGLRC;

namespace Quark {

	class OpenGLWin32GraphicsContext : public GraphicsContext
	{
	public:
		OpenGLWin32GraphicsContext(void* windowHandle);
		~OpenGLWin32GraphicsContext() override;

		void Init() override;
		void SwapBuffers() override;

	private:
		HWND m_WindowHandle;
		HGLRC m_Context;
	};
}

#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef HWND;
typedef HDC;
typedef HGLRC;

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
		HDC m_DeviceContext;
		HGLRC m_Context;
	};
}

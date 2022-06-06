#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include <Windows.h>

namespace Quark {

	class OpenGLWin32GraphicsContext final : public GraphicsContext
	{
	public:
		OpenGLWin32GraphicsContext(void* windowHandle);
		virtual ~OpenGLWin32GraphicsContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;
	};
}

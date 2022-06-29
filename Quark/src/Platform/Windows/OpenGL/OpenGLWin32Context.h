#pragma once

#include "Quark/Renderer/GraphicsContext.h"

#include <Windows.h>

namespace Quark {

	class OpenGLWin32Context final : public GraphicsContext
	{
	public:
		OpenGLWin32Context(void* windowHandle);
		virtual ~OpenGLWin32Context() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;
	};
}
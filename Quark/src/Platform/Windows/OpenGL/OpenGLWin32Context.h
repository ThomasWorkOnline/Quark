#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContextBase.h"

#include <Windows.h>

namespace Quark {

	class OpenGLWin32Context final : public OpenGLContextBase
	{
	public:
		OpenGLWin32Context(void* windowHandle);
		virtual ~OpenGLWin32Context() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetSwapInterval(int interval) override;

		// Non-Copyable
		OpenGLWin32Context(const OpenGLWin32Context&) = delete;
		OpenGLWin32Context& operator=(const OpenGLWin32Context&) = delete;

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;
	};
}

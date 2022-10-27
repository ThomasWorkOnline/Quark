#pragma once

#include "Platform/OpenGL/OpenGLContextBase.h"
#include <Windows.h>

namespace Quark {

	class OpenGLWin32Context final : public OpenGLContextBase
	{
	public:
		OpenGLWin32Context(void* windowHandle);
		virtual ~OpenGLWin32Context() final override;

		virtual void Init() final override;
		virtual void CreateSwapChain(const RenderPass* renderPass) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual ViewportExtent GetViewportExtent() const final override;

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;
	};
}

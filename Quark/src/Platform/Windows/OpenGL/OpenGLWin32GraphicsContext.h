#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct HWND__* HWND;
typedef struct HGLRC__* HGLRC;

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
		HGLRC m_Context;
	};
}

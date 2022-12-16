#pragma once

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLSwapChain.h"

typedef struct HWND__* HWND;
typedef struct HDC__* HDC;
typedef struct HGLRC__* HGLRC;

namespace Quark {

	class OpenGLWin32Context final : public OpenGLContext
	{
	public:
		OpenGLWin32Context(void* windowHandle);
		virtual ~OpenGLWin32Context() final override;

		virtual void Init(const SwapChainSpecification& spec) final override;

		virtual void SwapBuffers() final override;
		virtual void SetSwapInterval(int interval) final override;

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual SwapChain*        GetSwapChain() final override { return std::addressof(m_SwapChain); }
		virtual uint32_t          QuerySwapChainImageCount() const final override { return 1; }
		virtual ViewportExtent    QuerySwapExtent() const final override;

		virtual SwapPresentMode   ChooseSwapPresentMode(SwapPresentMode preferred) const final override { return SwapPresentMode::FIFO; }
		virtual SwapSurfaceFormat ChooseSurfaceFormat(SwapSurfaceFormat preferred) const final override;

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;

		OpenGLSwapChain m_SwapChain;
	};
}

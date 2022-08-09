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
		virtual void StartFrame() override;
		virtual void Submit() override;

		virtual void SwapBuffers() override;
		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override { return m_CommandBuffer; }

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;
	};
}

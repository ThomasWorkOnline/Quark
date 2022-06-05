#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class MetalGLFWGraphicsContext final : public GraphicsContext
	{
	public:
		MetalGLFWGraphicsContext(void* windowHandle);
		virtual ~MetalGLFWGraphicsContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

#pragma once

#include "Quark/Renderer/GraphicsContext.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class MetalContext final : public GraphicsContext
	{
	public:
		MetalContext(void* windowHandle);
		virtual ~MetalContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t viewportWidth, uint32_t viewportHeight) override {}

	private:
		GLFWwindow* m_WindowHandle;
	};
}

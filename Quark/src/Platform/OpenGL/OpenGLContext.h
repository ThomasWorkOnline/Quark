#pragma once

#include "Quark/Renderer/GraphicsContext.h"
#include "OpenGLContextBase.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLContext final : public OpenGLContextBase
	{
	public:
		OpenGLContext(void* windowHandle);
		virtual ~OpenGLContext() override;

		virtual void Init() override;
		virtual void StartFrame() override;
		virtual void Submit() override;

		virtual void SwapBuffers() override;
		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const override { return m_CommandBuffer; }

	private:
		GLFWwindow* m_WindowHandle;
	};
}

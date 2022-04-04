#pragma once

#include "Quark/Renderer/GraphicsContext.h"

namespace Quark {

	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(void* windowHandle);
		~OpenGLGraphicsContext() override;

		void Init() override;
		void SwapBuffers() override;

	private:
		void* m_WindowHandle;
	};
}

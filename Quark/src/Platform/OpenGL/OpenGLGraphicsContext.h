#pragma once

#include "Quark/Renderer/GraphicsContext.h"

namespace Quark {

	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(void* windowHandle);
		virtual ~OpenGLGraphicsContext() override;

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		void* m_WindowHandle;
	};
}

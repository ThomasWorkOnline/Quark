#pragma once

#include "Quark/Renderer/GraphicsContext.h"

namespace Quark {

	class OpenGLContextBase : public GraphicsContext
	{
	public:
		void Init();
		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

	protected:
		Ref<CommandBuffer> m_CommandBuffer;
	};
}

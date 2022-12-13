#pragma once

#include "Quark/Renderer/GraphicsContext.h"

namespace Quark {

	class OpenGLContext : public GraphicsContext
	{
	public:
		virtual void WaitUntilDeviceIdle() final override {}

		virtual void BeginFrame(uint32_t frameIndex) final override {}
		virtual void Flush(CommandBuffer* commandBuffer) final override {}
	};
}

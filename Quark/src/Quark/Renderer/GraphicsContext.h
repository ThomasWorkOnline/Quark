#pragma once

#include "Quark/Core/Core.h"
#include "CommandBuffer.h"

namespace Quark {

	class GraphicsContext : public Singleton<GraphicsContext>
	{
	public:
		GraphicsContext() : Singleton(this) {}
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void StartFrame() = 0;

		virtual void Submit() = 0;
		virtual void SwapBuffers() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		virtual uint32_t GetCurrentImageIndex() const = 0;
		virtual uint32_t GetSwapChainImageCount() const = 0;
		virtual CommandBuffer* GetCommandBuffer() = 0;

		static Scope<GraphicsContext> Create(void* windowHandle);
	};
}

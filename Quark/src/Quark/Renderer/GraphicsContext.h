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

		virtual const Ref<CommandBuffer>& GetCommandBuffer() const = 0;

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		static Scope<GraphicsContext> Create(void* windowHandle);
	};
}

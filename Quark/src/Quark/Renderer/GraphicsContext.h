#pragma once

#include "Quark/Core/Core.h"
#include "SwapChain.h"

namespace Quark {

	class GraphicsContext : public Singleton<GraphicsContext>
	{
	public:
		GraphicsContext() : Singleton(this) {}
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual SwapChain* GetSwapChain() = 0;

		static Scope<GraphicsContext> Create(void* windowHandle);
	};
}

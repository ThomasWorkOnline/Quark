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

		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight)
		{
			m_SwapChain->Resize(viewportWidth, viewportHeight);
		}

		SwapChain* GetSwapChain() const { return m_SwapChain.get(); }
		static Scope<GraphicsContext> Create(void* windowHandle);

	protected:
		Scope<SwapChain> m_SwapChain;
	};
}

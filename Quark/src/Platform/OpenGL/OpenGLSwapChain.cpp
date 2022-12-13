#include "qkpch.h"
#include "OpenGLSwapChain.h"

namespace Quark {

	OpenGLSwapChain::OpenGLSwapChain(const SwapChainSpecification& spec)
		: SwapChain(spec)
	{
	}

	void OpenGLSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Spec.Extent.Width = viewportWidth;
		m_Spec.Extent.Height = viewportHeight;
	}
}

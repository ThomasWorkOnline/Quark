#include "qkpch.h"
#include "OpenGLSwapChain.h"

namespace Quark {

	OpenGLSwapChain::OpenGLSwapChain(const SwapChainSpecification& spec)
		: SwapChain(spec)
	{
		FramebufferAttachmentSpecification fbSpec;
		fbSpec.Width = spec.Width;
		fbSpec.Height = spec.Height;
		fbSpec.Samples = spec.Samples;
		fbSpec.DataFormat = spec.SurfaceFormat.Format;
		m_ColorAttachment = CreateScope<OpenGLFramebufferAttachment>(fbSpec);
	}

	void OpenGLSwapChain::Resize(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		m_Spec.Width = viewportWidth;
		m_Spec.Height = viewportHeight;

		m_ColorAttachment->Resize(m_Spec.Width, m_Spec.Height);
	}

	const FramebufferAttachment* OpenGLSwapChain::GetColorAttachment(uint32_t index) const
	{
		return m_ColorAttachment.get();
	}
}

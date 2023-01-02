#pragma once

#include "Quark/Renderer/SwapChain.h"
#include "OpenGLFramebuffer.h"

namespace Quark {

	class OpenGLSwapChain : public SwapChain
	{
	public:
		OpenGLSwapChain() = default;
		OpenGLSwapChain(const SwapChainSpecification& spec);

		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) final override;

		virtual uint32_t GetBufferCount() const final override { return 1; }
		virtual uint32_t GetCurrentImageIndex() const final override { return 0; }

		virtual const FramebufferAttachment* GetColorAttachment(uint32_t index) const final override;

	private:
		Scope<OpenGLFramebufferAttachment> m_ColorAttachment;
	};
}

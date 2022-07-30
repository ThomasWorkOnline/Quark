#pragma once

#include "Quark/Renderer/SwapChain.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLSwapChain final : public SwapChain
	{
	public:
		OpenGLSwapChain(GLFWwindow* windowHandle);
		virtual ~OpenGLSwapChain();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetImageCount() const override { return 1; }
		virtual uint32_t GetCurrentImageIndex() const override { return 0; }

		virtual void AcquireNextImage() override {}
		virtual void Present() override;
		virtual void Resize(uint32_t viewportWidth, uint32_t viewportHeight) override;

		virtual const Ref<FramebufferAttachment>& GetAttachment(uint32_t imageIndex) const override;

	private:
		GLFWwindow* m_WindowHandle;
		uint32_t m_Width = 0, m_Height = 0;
		Ref<FramebufferAttachment> m_Attachment;
	};
}

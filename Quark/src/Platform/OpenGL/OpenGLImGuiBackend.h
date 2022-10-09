#pragma once

#include "Quark/ImGui/ImGuiBackend.h"

typedef struct GLFWwindow GLFWwindow;

namespace Quark {

	class OpenGLImGuiBackend final : public ImGuiBackend
	{
	public:
		OpenGLImGuiBackend(void* windowHandle);
		virtual ~OpenGLImGuiBackend() final override;

		virtual void Init(RenderPass* renderPass, CommandBuffer* commandBuffer) final override;
		virtual void NewFrame() final override;
		virtual void RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData) final override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}

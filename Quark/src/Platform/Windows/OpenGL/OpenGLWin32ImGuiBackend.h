#pragma once

#include "Quark/ImGui/ImGuiBackend.h"

#include <Windows.h>

namespace Quark {

	class OpenGLWin32ImGuiBackend : public ImGuiBackend
	{
	public:
		OpenGLWin32ImGuiBackend(void* windowHandle);
		virtual ~OpenGLWin32ImGuiBackend() final override;

		virtual void Init(RenderPass* renderPass, CommandBuffer* commandBuffer) final override;
		virtual void NewFrame() final override;
		virtual void RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData) final override;

	private:
		HWND m_WindowHandle;
	};
}

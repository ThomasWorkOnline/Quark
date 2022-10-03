#include "qkpch.h"
#include "OpenGLWin32ImGuiBackend.h"

namespace Quark {

	OpenGLWin32ImGuiBackend::OpenGLWin32ImGuiBackend(void* windowHandle)
		: m_WindowHandle(static_cast<HWND>(windowHandle))
	{
	}

	OpenGLWin32ImGuiBackend::~OpenGLWin32ImGuiBackend()
	{
	}

	void OpenGLWin32ImGuiBackend::Init(RenderPass* renderPass, CommandBuffer* commandBuffer)
	{
		(void)renderPass;
		(void)commandBuffer;
	}

	void OpenGLWin32ImGuiBackend::NewFrame()
	{
	}

	void OpenGLWin32ImGuiBackend::RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData)
	{
		(void)commandBuffer;
	}
}

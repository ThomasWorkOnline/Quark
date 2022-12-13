#include "qkpch.h"
#include "OpenGLWin32ImGuiBackend.h"

#include <Windows.h>
#include <glad/glad.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>

namespace Quark {

	OpenGLWin32ImGuiBackend::OpenGLWin32ImGuiBackend(void* windowHandle)
		: m_WindowHandle(static_cast<HWND>(windowHandle))
	{
	}

	OpenGLWin32ImGuiBackend::~OpenGLWin32ImGuiBackend()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}

	void OpenGLWin32ImGuiBackend::Init(RenderPass* renderPass, CommandBuffer* commandBuffer)
	{
		(void)renderPass;
		(void)commandBuffer;

		bool initialized = ImGui_ImplWin32_Init(m_WindowHandle);
		QK_CORE_ASSERT(initialized, "Failed to initialize the ImGui Layer");

		initialized = ImGui_ImplOpenGL3_Init("#version 410 core");
		QK_CORE_ASSERT(initialized, "Failed to initialize OpenGL");
	}

	void OpenGLWin32ImGuiBackend::NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
	}

	void OpenGLWin32ImGuiBackend::RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData)
	{
		(void)commandBuffer;

		GLboolean gammaEnabled = glIsEnabled(GL_FRAMEBUFFER_SRGB);
		glDisable(GL_FRAMEBUFFER_SRGB);

		ImGui_ImplOpenGL3_RenderDrawData(drawData);

		if (gammaEnabled)
			glEnable(GL_FRAMEBUFFER_SRGB);
		
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			HGLRC backupCurrentContext = wglGetCurrentContext();
			HDC dc = wglGetCurrentDC();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			wglMakeCurrent(dc, backupCurrentContext);
		}
	}
}

#include <backends/imgui_impl_win32.cpp>

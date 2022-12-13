#include "qkpch.h"
#include "OpenGLImGuiBackend.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_opengl3.h>

namespace Quark {

	OpenGLImGuiBackend::OpenGLImGuiBackend(void* windowHandle)
		: m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
	{
	}

	OpenGLImGuiBackend::~OpenGLImGuiBackend()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void OpenGLImGuiBackend::Init(RenderPass* renderPass, CommandBuffer* commandBuffer)
	{
		(void)renderPass;
		(void)commandBuffer;

		bool initialized = ImGui_ImplGlfw_InitForOpenGL(m_WindowHandle, true);
		QK_CORE_ASSERT(initialized, "Failed to initialize the ImGui Layer");

		initialized = ImGui_ImplOpenGL3_Init("#version 410 core");
		QK_CORE_ASSERT(initialized, "Failed to initialize OpenGL");
	}

	void OpenGLImGuiBackend::NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void OpenGLImGuiBackend::RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData)
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
			GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backupCurrentContext);
		}
	}
}

#include <backends/imgui_impl_opengl3.cpp>

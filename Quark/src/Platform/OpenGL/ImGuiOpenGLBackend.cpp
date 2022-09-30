#include "qkpch.h"

#include "Quark/Core/Core.h"
#include "Quark/ImGui/ImGuiBackend.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3.cpp>

namespace Quark {

	ImGuiBackend CreateImGuiOpenGLBackend()
	{
		ImGuiBackend backend;
		backend.Init = [](void* windowHandle)
		{
			bool initialized = ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(windowHandle), true);
			QK_CORE_ASSERT(initialized, "Failed to initialize the ImGui Layer");

			initialized = ImGui_ImplOpenGL3_Init("#version 410 core");
			QK_CORE_ASSERT(initialized, "Failed to initialize OpenGL");
		};

		backend.Shutdown = []()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		};

		backend.NewFrame = []()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		};

		backend.RenderDrawData = [](ImDrawData* drawData)
		{
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
		};

		return backend;
	}
}

#include "qkpch.h"

#include "Quark/Core/Core.h"
#include "Quark/ImGui/ImGuiBackend.cpp"

#include <glad/glad.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_opengl3.cpp>
#include <backends/imgui_impl_glfw.cpp>

namespace Quark {

	void BindImGuiOpenGLBackend()
	{
		ImGuiBackend::Init = [](void* windowHandle)
		{
			bool initialized = ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(windowHandle), true);
			QK_CORE_ASSERT(initialized, "Failed to initialize the ImGui Layer");

			initialized = ImGui_ImplOpenGL3_Init("#version 410 core");
			QK_CORE_ASSERT(initialized, "Failed to initialize OpenGL");
		};

		ImGuiBackend::Shutdown = []()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		};

		ImGuiBackend::NewFrame = []()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		};

		ImGuiBackend::RenderDrawData = [](ImDrawData* drawData)
		{
			ImGui_ImplOpenGL3_RenderDrawData(drawData);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backupCurrentContext);
			}
		};
	}
}

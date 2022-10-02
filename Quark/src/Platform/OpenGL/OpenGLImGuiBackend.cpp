#include "qkpch.h"

#include "Quark/Core/Core.h"
#include "Quark/ImGui/ImGuiBackend.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.cpp>

namespace Quark {

	class OpenGLImGuiBackend final : public ImGuiBackend
	{
	public:
		OpenGLImGuiBackend() = default;

		virtual void Init(void* windowHandle) final override
		{
			bool initialized = ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(windowHandle), true);
			QK_CORE_ASSERT(initialized, "Failed to initialize the ImGui Layer");

			initialized = ImGui_ImplOpenGL3_Init("#version 410 core");
			QK_CORE_ASSERT(initialized, "Failed to initialize OpenGL");
		}

		virtual ~OpenGLImGuiBackend() final override
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}

		virtual void NewFrame() final override
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}

		virtual void RenderDrawData(ImDrawData* drawData) final override
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
		}
	};

	Scope<ImGuiBackend> CreateImGuiOpenGLBackend()
	{
		return CreateScope<OpenGLImGuiBackend>();
	}
}

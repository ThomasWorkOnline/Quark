#include "qkpch.h"
#include "ImGuiLayer.h"

#include "ImGuiBackend.h"

#include "Quark/Core/Application.h"
#include "Quark/Renderer/Renderer.h"

#include <imgui.h>

namespace Quark {

	static Scope<ImGuiBackend> s_ImGuiBackend = nullptr;

	ImGuiLayer::ImGuiLayer(Application* app)
		: Layer(app)
	{
#ifdef QK_DEBUG
		IMGUI_CHECKVERSION();
#endif
		auto* window = app->GetWindow();
		s_ImGuiBackend = ImGuiBackend::Create(window->GetNativeWindow(), window->IsNative());

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsDark();

		//SetStyleDarkTheme();

		auto& coreDir = app->GetOptions().CoreDir;

		float fontSize = 16.0f;
		io.Fonts->AddFontFromFileTTF((coreDir / "assets/fonts/Segoe UI/segoeuib.ttf").string().c_str(), fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF((coreDir / "assets/fonts/Segoe UI/segoeui.ttf").string().c_str(), fontSize);

		auto* renderPass = Renderer::GetRenderPass();
		auto* commandBuffer = Renderer::GetCommandBuffer();
		s_ImGuiBackend->Init(renderPass, commandBuffer);
	}

	ImGuiLayer::~ImGuiLayer()
	{
		s_ImGuiBackend.reset();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::BeginFrame()
	{
		s_ImGuiBackend->NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		ImGui::Render();
		auto* commandBuffer = Renderer::GetCommandBuffer();
		s_ImGuiBackend->RenderDrawData(commandBuffer, ImGui::GetDrawData());
	}

	void ImGuiLayer::SetStyleDarkTheme()
	{
		auto& style = ImGui::GetStyle();

		auto& colors = style.Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}

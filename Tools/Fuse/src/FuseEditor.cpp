#include "FuseEditor.h"

#include <imgui.h>

namespace Quark {

	FuseEditor::FuseEditor(const ApplicationOptions& options)
		: Application(options)
	{
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		float fontSize = 16.0f;
		io.Fonts->AddFontFromFileTTF("assets/fonts/Segoe UI/segoeuib.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Segoe UI/segoeui.ttf", fontSize);
	}

	void FuseEditor::OnUpdate(Timestep ts)
	{
	}

	void FuseEditor::OnRender()
	{
		static constexpr ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if constexpr ((bool)(dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode))
			windowFlags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::Begin("DockSpace", nullptr, windowFlags);
		{
			ImGui::PopStyleVar(3);

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspaceId = ImGui::GetID("DockSpace");
				ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit", "Alt+F4"))
						Stop();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}
}

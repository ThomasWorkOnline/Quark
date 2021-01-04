#include "Application.h"

#include "Core.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/RenderCommand.h"

#include "../Tools/Colorimetry.h"

// Only to get time
#include <GLFW/glfw3.h>

namespace Entropy {

	Application* Application::s_Instance = nullptr;

	Application::Application(uint32_t width, uint32_t height, const char* title)
	{
		s_Instance = this;

		m_Window = Window::Create(width, height, title);
		m_Window->SetEventCallback(NT_ATTACH_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);

		std::string appendedTitle = " - ";
		appendedTitle.append(RenderingAPI::GetName());
		Application::GetWindow().AppendTitle(appendedTitle);

		NT_INFO(RenderCommand::GetSpecification());
		Renderer::Init();
	}

	Application::~Application()
	{
		Renderer::Dispose();

		NT_TRACE("Hey! Come back next time.");
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(NT_ATTACH_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(NT_ATTACH_EVENT_FN(Application::OnWindowResize));

		// Dispatch all other not already handled events
		if (!e.Handled) OnApplicationEvent(e);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		RenderCommand::SetClearColor(EncodeSRGB(glm::vec4(0.0862f, 0.3764f, 0.6549f, 1.0f)));

		OnCreate();

		float fStartTime = 0.0f;

		while (m_Running)
		{
			RenderCommand::Clear();

			float fEndTime = (float)glfwGetTime();
			OnUpdate(fEndTime - fStartTime);
			fStartTime = fEndTime;

			m_Window->OnUpdate();
		}
	}
}

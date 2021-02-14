#include "Application.h"

#include "Core.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/RenderCommand.h"

#include "../Tools/Colorimetry.h"

// Only to get time
#include <GLFW/glfw3.h>

namespace Entropy {

	Application* Application::s_Instance = nullptr;

	Application::Application(uint32_t width, uint32_t height, const std::string& title)
	{
		s_Instance = this;

		m_Window = Window::Create(width, height, title);
		m_Window->SetEventCallback(NT_ATTACH_EVENT_FN(Application::OnEventInternal));

		std::string appendedTitle = " - ";
		appendedTitle.append(RenderingAPI::GetName());
		m_Window->AppendTitle(appendedTitle);

		NT_INFO(RenderCommand::GetSpecification());
		Renderer::Init();
	}

	Application::~Application()
	{
		OnDestroy();

		Renderer::Dispose();

		NT_TRACE("Hey! Come back next time.");
	}

	void Application::OnEventInternal(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(NT_ATTACH_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(NT_ATTACH_EVENT_FN(Application::OnWindowResize));

		// Dispatch all other not already handled events
		if (!e.Handled) OnEvent(e);
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
		OnCreate();

		float fStartTime = 0.0f;

		while (m_Running)
		{
			RenderCommand::SetClearColor(EncodeSRGB({ 210.0f / 255.0f, 230.0f / 255.0f, 244.0f / 255.0f, 1.0f }));
			RenderCommand::Clear();

			float fEndTime = (float)glfwGetTime();
			OnUpdate(fEndTime - fStartTime);
			fStartTime = fEndTime;

			m_Window->OnUpdate();
		}
	}
}

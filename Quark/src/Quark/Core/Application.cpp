#include "Application.h"

#include "Core.h"

#include "../Audio/AudioEngine.h"
#include "../Tools/Colorimetry.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/RenderCommand.h"

#include <ctime>

namespace Quark {

	Application* Application::s_Instance = nullptr;

	Application::Application(uint32_t width, uint32_t height, const std::string& title)
	{
		QK_TIME_SCOPE_DEBUG(Application::Application);

		s_Instance = this;

		m_Window = Window::Create(width, height, title);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEventInternal));
		m_Window->SetVSync(true);

		std::string appendedTitle = " - ";
		appendedTitle.append(RenderingAPI::GetName());
		m_Window->AppendTitle(appendedTitle);

		QK_CORE_INFO(RenderCommand::GetSpecification());
		Renderer::Initialize();

		AudioEngine::Initialize();
	}

	Application::~Application()
	{
		QK_TIME_SCOPE_DEBUG(Application::~Application);

		AudioEngine::Dispose();
		Renderer::Dispose();

		QK_CORE_TRACE("Hey! Come back next time.");
	}

	void Application::OnEventInternal(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>(ATTACH_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(Application::OnWindowResize));

		// Dispatch all other not already handled events
		if (!e.Handled) OnEvent(e);
	}

	bool Application::OnWindowClose(WindowClosedEvent& e)
	{
		Stop();
		return true;
	}

	bool Application::OnWindowResize(WindowResizedEvent& e)
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

		auto tStart = std::chrono::steady_clock::now();

		while (m_Running)
		{
			RenderCommand::SetClearColor(EncodeSRGB({ 0.1f, 0.1f, 0.1f, 1.0f }));
			RenderCommand::Clear();

			AudioEngine::OnUpdate();

			auto tNow = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsedTime = tNow - tStart;
			tStart = std::chrono::steady_clock::now();

			OnUpdate(elapsedTime.count());

			m_Window->OnUpdate();
		}

		OnDestroy();
	}
}

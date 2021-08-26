#include "Application.h"

#include "Core.h"

#include "../Audio/AudioEngine.h"
#include "../Renderer/FontEngine.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/RenderCommand.h"
#include "../Tools/Colorimetry.h"

#include <ctime>

namespace Quark {

	Application* Application::s_Instance = nullptr;
	std::thread::id Application::s_AppMainThreadId;

	Application::Application(uint32_t width, uint32_t height, const std::string& title, ApplicationFlag flags)
		: m_Flags(flags)
	{
		QK_TIME_SCOPE_DEBUG(Application::Application);

		s_Instance = this;
		s_AppMainThreadId = std::this_thread::get_id();

		m_Window = Window::Create(width, height, title);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEventInternal));

		if (HasFlag(ApplicationFlag::ShowAPI))
		{
			std::string appendedTitle = " - " + std::string(RenderingAPI::GetName());
			m_Window->AppendTitle(appendedTitle);
		}

		QK_CORE_INFO(RenderCommand::GetSpecification());
		Renderer::Initialize();
		RenderCommand::SetClearColor(EncodeSRGB({ 0.1f, 0.1f, 0.1f, 1.0f }));

		AudioEngine::Initialize();
		FontEngine::Initialize();
	}

	Application::~Application()
	{
		QK_TIME_SCOPE_DEBUG(Application::~Application);

		Renderer::Dispose();
		AudioEngine::Dispose();
		FontEngine::Dispose();
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

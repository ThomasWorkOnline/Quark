#include "Application.h"

#include "Core.h"

#include "../Audio/AudioEngine.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/RenderCommand.h"
#include "../Renderer/DeferredObjectManager.h"
#include "../Tools/Colorimetry.h"

#include <ctime>

namespace Quark {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationOptions& options)
		: m_Options(options)
	{
		QK_TIME_SCOPE_DEBUG(Application::Application);

		s_Instance = this;
		m_AppMainThreadId = std::this_thread::get_id();

		RenderingApi = RenderingAPI::Create(options.Api);

		WindowSpecification spec = {
			options.Width, options.Height, options.Title, 4
		};

		m_Window = Window::Create(spec);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEvent));

		Renderer::Initialize(m_Window->GetWidth(), m_Window->GetHeight());
		AudioEngine::Initialize();

		if (options.HasFlag(ShowApiInWindowTitle))
		{
			std::string appendedTitle = " - " + std::string(RenderingAPI::GetAPIName());
			m_Window->AppendTitle(appendedTitle);
		}

		RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });
		QK_CORE_INFO(RenderCommand::GetSpecification());
	}

	Application::~Application()
	{
		QK_CORE_TRACE("Initiating shutdown...");
		QK_TIME_SCOPE_DEBUG(Application::~Application);

		for (auto layer : m_Layers)
		{
			delete layer;
		}

		DeferredObjectManager::ReleaseRenderObjects();

		Renderer::Dispose();
		AudioEngine::Dispose();

		s_Instance = nullptr;
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		auto tStart = std::chrono::steady_clock::now();

		while (m_Running)
		{
			RenderCommand::Clear();

			auto tNow = std::chrono::steady_clock::now();
			float elapsedTime = std::chrono::duration<float>(tNow - tStart).count();
			tStart = std::chrono::steady_clock::now();

			m_TotalTime += elapsedTime;

			for (auto layer : m_Layers)
			{
				layer->OnUpdate(elapsedTime);
			}

			m_Window->OnUpdate();

			AudioEngine::OnUpdate();
			DeferredObjectManager::ReleaseRenderObjects();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>(ATTACH_EVENT_FN(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(Application::OnWindowResized));

		// Dispatch all other not already handled events
		for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); it++)
		{
			if (e.Handled)
				break;

			auto layer = *it;
			layer->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
	}

	bool Application::OnWindowClosed(WindowClosedEvent& e)
	{
		Stop();
		return false;
	}

	bool Application::OnWindowResized(WindowResizedEvent& e)
	{
		Renderer::OnWindowResized(e.GetWidth(), e.GetHeight());
		return false;
	}
}

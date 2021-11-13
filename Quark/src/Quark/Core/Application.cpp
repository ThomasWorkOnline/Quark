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

		RenderCommand::CreateRenderingAPI(options.Api);

		m_Window = Window::Create(options.Width, options.Height, options.Title);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEventInternal));

		Renderer::Initialize(m_Window->GetWidth(), m_Window->GetHeight());

		if (options.HasFlag(EnableBatchRenderer))
			Renderer::InitializeBatchRenderer();

		if (options.HasFlag(EnableAudioEngine))
			AudioEngine::Initialize();

		if (options.HasFlag(ShowApiInWindowTitle))
		{
			std::string appendedTitle = " - " + std::string(RenderingAPI::GetName());
			m_Window->AppendTitle(appendedTitle);
		}

		RenderCommand::SetClearColor(EncodeSRGB({ 0.01f, 0.01f, 0.01f, 1.0f }));
		QK_CORE_INFO(RenderCommand::GetSpecification());
	}

	Application::~Application()
	{
		QK_TIME_SCOPE_DEBUG(Application::~Application);

		if (m_Options.HasFlag(EnableBatchRenderer))
			Renderer::Dispose();

		if (m_Options.HasFlag(EnableAudioEngine))
			AudioEngine::Dispose();

		DeferredObjectManager::ReleaseRenderObjects();
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

		return false;
	}

	bool Application::OnWindowResize(WindowResizedEvent& e)
	{
		Renderer::OnWindowResized(e.GetWidth(), e.GetHeight());

		return false;
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

			if (m_Options.HasFlag(EnableAudioEngine))
				AudioEngine::OnUpdate();

			auto tNow = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsedTime = tNow - tStart;
			tStart = std::chrono::steady_clock::now();

			OnUpdate(elapsedTime.count());

			m_Window->OnUpdate();

			DeferredObjectManager::ReleaseRenderObjects();
		}
	}
}

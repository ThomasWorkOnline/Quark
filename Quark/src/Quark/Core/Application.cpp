#include "qkpch.h"
#include "Application.h"

#include "Core.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/Renderer2D.h"
#include "Quark/Renderer/SceneRenderer.h"
#include "Quark/Renderer/RenderCommand.h"

#include <ctime>

namespace Quark {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationOptions& options)
		: m_Options(options)
	{
		Initialize();
	}

	Application::~Application()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_TRACE("Initiating shutdown...");

		for (size_t i = 0; i < m_Layers.size(); i++)
			delete m_Layers[i];

		SceneRenderer::Shutdown();

		//Renderer2D::Dispose();
		Renderer::Dispose();
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		m_Running = true;
		auto tStart = std::chrono::steady_clock::now();

		while (m_Running)
		{
			{
				auto tNow = std::chrono::steady_clock::now();
				float elapsedTime = std::chrono::duration<float>(tNow - tStart).count();
				tStart = tNow;

				m_TotalTime += elapsedTime;
				OnUpdate(elapsedTime);

				// Don't use iterator based for loops; we might modify the layer stack in OnUpdate()
				for (size_t i = 0; i < m_Layers.size(); i++)
					m_Layers[i]->OnUpdate(elapsedTime);
			}

			if (!m_Minimized)
			{
				SceneRenderer::OnRender();
				OnRender();

				for (size_t i = 0; i < m_Layers.size(); i++)
					m_Layers[i]->OnRender();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::Initialize()
	{
		QK_PROFILE_FUNCTION();

		s_Instance = this;
		m_AppMainThreadId = std::this_thread::get_id();

		WindowSpecification spec = {
			m_Options.AppName, m_Options.Width, m_Options.Height, 4
		};

		m_Window = Window::Create(spec);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEventInternal));

		m_AudioOutputDevice = AudioOutputDevice::Create();
		QK_CORE_INFO("Opened audio device: {0}", m_AudioOutputDevice->GetDeviceName());

		Renderer::Initialize();
		QK_CORE_INFO(GraphicsAPI::Instance->GetSpecification());

		//Renderer2D::Initialize();

		SceneRenderer::Initialize();
		SceneRenderer::OnViewportResized(m_Window->GetWidth(), m_Window->GetHeight());

		if (m_Options.HasFlag(ShowApiInWindowTitle))
		{
			std::string appendedTitle = " - " + std::string(GraphicsAPI::Instance->GetName());
			m_Window->AppendTitle(appendedTitle);
		}
	}

	void Application::OnEventInternal(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>(ATTACH_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(OnWindowResized));
		dispatcher.Dispatch<WindowMinimizedEvent>(ATTACH_EVENT_FN(OnWindowMinimized));
		dispatcher.Dispatch<WindowRestoredEvent>(ATTACH_EVENT_FN(OnWindowRestored));

		if (!e.Handled)
			OnEvent(e);

		// Dispatch all other not already handled events
		for (size_t i = m_Layers.size(); i > 0; i--)
		{
			if (e.Handled)
				break;

			m_Layers[i - 1]->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);

		if (it != m_Layers.end())
			m_Layers.erase(it);
	}

	bool Application::OnWindowClosed(WindowClosedEvent& e)
	{
		Stop();
		return false;
	}

	bool Application::OnWindowResized(WindowResizedEvent& e)
	{
		SceneRenderer::OnViewportResized(e.GetWidth(), e.GetHeight());
		return false;
	}

	bool Application::OnWindowMinimized(WindowMinimizedEvent& e)
	{
		m_Minimized = true;
		return false;
	}

	bool Application::OnWindowRestored(WindowRestoredEvent& e)
	{
		m_Minimized = false;
		return false;
	}
}

#include "qkpch.h"
#include "Application.h"

#include "Core.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/Renderer2D.h"
#include "Quark/Renderer/SceneRenderer.h"
#include "Quark/Renderer/RenderCommand.h"

#include <ctime>

namespace Quark {

	Application::Application(const ApplicationOptions& options) : Singleton(this),
		m_Options(options)
	{
		Initialize();
	}

	Application::~Application()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_TRACE("Initiating shutdown...");

		for (size_t i = 0; i < m_Layers.size(); i++)
			delete m_Layers[i];

		Renderer2D::Dispose();
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

			OnRender();

			for (size_t i = 0; i < m_Layers.size(); i++)
				m_Layers[i]->OnRender();

			m_Window->OnUpdate();
		}
	}

	void Application::Initialize()
	{
		QK_PROFILE_FUNCTION();

		m_AppMainThreadId = std::this_thread::get_id();

		WindowSpecification spec = {
			m_Options.AppName.empty() ? "Quark Engine" : m_Options.AppName, m_Options.Width, m_Options.Height, 4
		};

		m_Window = Window::Create(spec);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEventInternal));

		m_AudioOutputDevice = AudioOutputDevice::Create();
		QK_CORE_INFO("Opened audio device: {0}", m_AudioOutputDevice->GetDeviceName());

		Renderer::Initialize();
		Renderer2D::Initialize();
		QK_CORE_INFO(GraphicsAPI::Instance->GetSpecification());

		if (m_Options.HasFlag(ShowApiInWindowTitle))
		{
			auto title = GraphicsAPI::Instance->GetName();
			m_Window->AppendTitle(" - ").AppendTitle(title);
		}
	}

	void Application::OnEventInternal(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>([&](auto& e) { Stop(); });
		dispatcher.Dispatch<WindowMinimizedEvent>([&](auto& e) { m_Minimized = true; });
		dispatcher.Dispatch<WindowRestoredEvent>([&](auto& e) { m_Minimized = false; });

		OnEvent(e);

		if (e.Handled)
			return;

		// Dispatch all other not already handled events
		for (size_t i = m_Layers.size(); i > 0; i--)
		{
			m_Layers[i - 1]->OnEvent(e);

			if (e.Handled)
				break;
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
}

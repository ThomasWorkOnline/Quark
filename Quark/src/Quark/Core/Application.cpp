#include "qkpch.h"
#include "Application.h"

#include "Quark/ImGui/ImGuiLayer.h"

#include <ctime>
#include <filesystem>

namespace Quark {

	Application::Application(const ApplicationSpecification& spec) : Singleton(this),
		m_Spec(spec)
	{
		QK_PROFILE_FUNCTION();

		m_AppMainThreadId = std::this_thread::get_id();

		if (!m_Spec.AssetDir.empty())
			std::filesystem::current_path(m_Spec.AssetDir);

		Renderer::Configure(m_Spec.GraphicsAPI);

		WindowSpecification windowSpec = {
			m_Spec.AppName.empty() ? "Quark Engine" : m_Spec.AppName, m_Spec.Width, m_Spec.Height, m_Spec.Samples, m_Spec.VSync
		};

		m_Window = Window::Create(windowSpec);
		m_Window->SetEventCallback(ATTACH_EVENT_FN(Application::OnEventInternal));

		auto* graphicsContext = m_Window->GetGraphicsContext();
		Renderer::Initialize(graphicsContext->GetSwapChain());

		QK_CORE_INFO(Renderer::GetSpecification());

		m_Window->AppendTitle(fmt::format(" - {0} ({1})", QK_PLATFORM_NAME, Renderer::GetAPIName()));

		if (m_Spec.HasFlags(ApplicationFlagBits::LaunchRenderer2D))
		{
			QK_CORE_TRACE("Initializing Renderer2D...");
			Renderer2D::Initialize();
		}

		if (m_Spec.HasFlags(ApplicationFlagBits::EnableAudioOutputDevice))
		{
			QK_CORE_TRACE("Opening audio device...");
			m_AudioOutputDevice = AudioOutputDevice::Create();
			QK_CORE_INFO("Opened audio device: {0}", m_AudioOutputDevice->GetDeviceName());
		}

		m_ImGuiLayer = new ImGuiLayer(this);
	}

	Application::~Application()
	{
		QK_PROFILE_FUNCTION();

		QK_CORE_TRACE("Initiating shutdown...");

		for (size_t i = 0; i < m_Layers.size(); i++)
			delete m_Layers[i];

		delete m_ImGuiLayer;

		Renderer2D::Dispose();
		Renderer::Dispose();
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::Crash(std::exception& e)
	{
		OnCrash(e);

		(void)e;
		QK_CORE_ASSERT(false, e.what());
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

			{
				Renderer::BeginFrame();
				m_ImGuiLayer->BeginFrame();
				
				OnRender();

				for (auto layer : m_Layers)
					layer->OnRender();

				m_ImGuiLayer->EndFrame();
				Renderer::EndFrame();
			}

			m_Window->OnUpdate();
		}

		Renderer::WaitUntilDeviceIdle();
	}

	void Application::OnEventInternal(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>([&](auto& e) { Stop(); });
		dispatcher.Dispatch<WindowMinimizedEvent>([&](auto& e) { m_Minimized = true; });
		dispatcher.Dispatch<WindowRestoredEvent>([&](auto& e) { m_Minimized = false; });
		dispatcher.Dispatch<WindowResizedEvent>([&](WindowResizedEvent& e)
		{
			m_Spec.Width = e.GetWidth();
			m_Spec.Height = e.GetHeight();
		});

		dispatcher.Dispatch<ViewportResizedEvent>([](ViewportResizedEvent& e)
		{
			Renderer::SetViewport(e.GetWidth(), e.GetHeight());
		});

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

	AudioOutputDevice* Application::OpenAudioOutputDevice(const char* deviceName)
	{
		m_AudioOutputDevice = AudioOutputDevice::Create(deviceName);
		return m_AudioOutputDevice.get();
	}

	bool Application::DefaultEventHandler(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));

		return false;
	}

	void Application::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == m_Spec.FullscreenKey)
		{
			m_Window->SetFullscreen(!m_Window->IsFullscreen());
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

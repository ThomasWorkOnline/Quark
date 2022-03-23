#pragma once

#include "Core.h"

#include "AppOptions.h"
#include "Layer.h"
#include "Window.h"

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Events/ApplicationEvent.h"
#include "Quark/Renderer/RenderingAPI.h"

#include <thread>

namespace Quark {

	class Application
	{
	public:
		Application(const ApplicationOptions& options = {});
		virtual ~Application();

		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnEvent(Event& e) {}

		void Run();
		void Stop();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		float GetAppRunningTime() const { return m_TotalTime; }
		std::thread::id GetThreadId() const { return m_AppMainThreadId; }

		const Window& GetWindow() const { return *m_Window; }
		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instance; }

	private:
		void Initialize();
		void OnEventInternal(Event& e);

		bool OnWindowClosed(WindowClosedEvent& e);
		bool OnWindowResized(WindowResizedEvent& e);

	private:
		static Application* s_Instance;
		std::thread::id m_AppMainThreadId;

		ApplicationOptions m_Options;
		Scope<Window> m_Window;
		Scope<AudioDevice> m_AudioDevice;
		std::vector<Layer*> m_Layers;

		float m_TotalTime = 0.0f;
		bool m_Running = false;
	};
}

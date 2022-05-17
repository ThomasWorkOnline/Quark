#pragma once

#include "Core.h"

#include "Layer.h"
#include "Window.h"

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Events/ApplicationEvent.h"
#include "Quark/Renderer/RenderingAPI.h"

#include <thread>

namespace Quark {

	enum ApplicationFlag
	{
		ApplicationNoFlags = 0,
		ShowApiInWindowTitle = BIT(0)
	};

	struct ApplicationOptions
	{
		uint32_t Width = 1280, Height = 720;
		std::string Title = "Quark Engine";
		ApplicationFlag Flags = ApplicationNoFlags;

		bool HasFlag(ApplicationFlag flag) const { return Flags & flag; }

		ApplicationOptions() = default;
		ApplicationOptions(
			uint32_t width, uint32_t height,
			const std::string& title,
			ApplicationFlag flags = ApplicationNoFlags
		)
			: Width(width), Height(height),
			Title(title), Flags(flags) {}
	};

	class Application
	{
	public:
		Application(const ApplicationOptions& options = {});
		virtual ~Application();

		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnRender() {}
		virtual void OnEvent(Event& e) {}

		void Run();
		void Stop();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		float GetAppRunningTime() const { return m_TotalTime; }
		std::thread::id GetThreadId() const { return m_AppMainThreadId; }

		const Window& GetWindow() const { return *m_Window; }
		Window& GetWindow() { return *m_Window; }

		const AudioOutputDevice& GetAudioOutputDevice() const { return *m_AudioOutputDevice; }
		AudioOutputDevice& GetAudioOutputDevice() { return *m_AudioOutputDevice; }

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
		Scope<AudioOutputDevice> m_AudioOutputDevice;
		std::vector<Layer*> m_Layers;

		float m_TotalTime = 0.0f;
		bool m_Running = false;
	};
}

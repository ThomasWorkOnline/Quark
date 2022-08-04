#pragma once

#include "Core.h"

#include "Layer.h"
#include "Window.h"

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Event/ApplicationEvent.h"
#include "Quark/Renderer/GraphicsAPI.h"

#include <thread>
#include <filesystem>

namespace Quark {

	enum ApplicationFlag
	{
		ApplicationFlagEmpty = 0,
		ShowApiInWindowTitle = BIT(0)
	};

	struct ApplicationOptions
	{
		uint32_t Width = 1280, Height = 720;
		std::string AppName;

		ApplicationFlag Flags{};
		std::filesystem::path AssetDir;

		bool HasFlag(ApplicationFlag flag) const { return Flags & flag; }

		ApplicationOptions() = default;
		ApplicationOptions(
			uint32_t width, uint32_t height,
			const std::string& title,
			ApplicationFlag flags = {})
			: Width(width), Height(height), AppName(title), Flags(flags) {}
	};

	class Application : public Singleton<Application>
	{
	public:
		Application(const ApplicationOptions& options = {});
		virtual ~Application();

		virtual void OnEvent(Event& e) {}
		virtual void OnRender() {}
		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnCrash() {}

		void Run();
		void Stop();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		float GetAppRunningTime() const { return m_TotalTime; }
		std::thread::id GetThreadId() const { return m_AppMainThreadId; }

		const ApplicationOptions& GetOptions() const { return m_Options; }

		const Window& GetWindow() const { return *m_Window; }
		Window& GetWindow() { return *m_Window; }

		const AudioOutputDevice& GetAudioOutputDevice() const { return *m_AudioOutputDevice; }
		AudioOutputDevice& GetAudioOutputDevice() { return *m_AudioOutputDevice; }

	private:
		void OnEventInternal(Event& e);

	private:
		ApplicationOptions m_Options;
		Scope<Window> m_Window;
		Scope<AudioOutputDevice> m_AudioOutputDevice;
		std::thread::id m_AppMainThreadId;
		std::vector<Layer*> m_Layers;

		float m_TotalTime = 0.0f;
		bool m_Minimized = false;
		bool m_Running = false;
	};
}

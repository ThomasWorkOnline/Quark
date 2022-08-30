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

	enum class ApplicationFlagBits : uint8_t
	{
		None                    = 0,
		EnableAudioOutputDevice = BIT(0)
	};

	struct ApplicationOptions
	{
		uint32_t Width = 1280, Height = 720;
		std::string AppName;

		std::filesystem::path AssetDir;
		std::filesystem::path CoreAssetDir;

		ApplicationFlagBits Flags{};
		KeyCode FullscreenKey = KeyCode::F11;
		RHI GraphicsAPI = GraphicsAPI::GetDefaultRHIForPlatform();

		bool HasFlag(ApplicationFlagBits flag) const { return (uint8_t)Flags & (uint8_t)flag; }
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
		ApplicationOptions& GetOptions() { return m_Options; }

		Window* GetWindow() const { return m_Window.get(); }
		AudioOutputDevice* GetAudioOutputDevice() const { return m_AudioOutputDevice.get(); }

		AudioOutputDevice* OpenAudioOutputDevice(const char* deviceName = nullptr);

	protected:
		bool DefaultEventHandler(Event& e);

	private:
		void OnEventInternal(Event& e);
		void OnKeyPressed(KeyPressedEvent& e);

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

#pragma once

#include "Core.h"
#include "Layer.h"
#include "Window.h"

#include "Quark/Audio/AudioDevice.h"
#include "Quark/Event/ApplicationEvent.h"

#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/Renderer2D.h"

#include <thread>
#include <filesystem>

namespace Quark {

	class ImGuiLayer;

	enum ApplicationFlagBits : uint8_t
	{
		ApplicationFlagBitsNone = 0,
		EnableAudioOutputDevice = BIT(0)
	};

	struct CommandLineArguments
	{
		int    Argc = 0;
		char** Argv = nullptr;

		CommandLineArguments() = default;
		CommandLineArguments(int argc, char** argv)
			: Argc(argc), Argv(argv)
		{
		}
	};

	struct ApplicationOptions
	{
		uint32_t    Width = 1280, Height = 720;
		std::string AppName;

		std::filesystem::path CoreDir;
		std::filesystem::path AssetDir;

		CommandLineArguments  CommandLineArgs;
		ApplicationFlagBits   Flags{};
		KeyCode FullscreenKey = KeyCode::F11;

		RHI GraphicsAPI = Renderer::GetPreferredRHI();

		bool HasFlag(ApplicationFlagBits flag) const { return Flags & flag; }
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

		ImGuiLayer* m_ImGuiLayer = nullptr;

		std::thread::id m_AppMainThreadId;
		std::vector<Layer*> m_Layers;

		float m_TotalTime = 0.0f;
		bool m_Minimized = false;
		bool m_Running = false;
	};
}

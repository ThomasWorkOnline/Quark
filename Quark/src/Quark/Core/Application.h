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

	enum class ApplicationFlagBits : uint32_t
	{
		None = 0,
		LaunchRenderer2D        = BIT(0),
		EnableAudioOutputDevice = BIT(1)
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

	struct ApplicationSpecification
	{
		std::string           AppName;
		std::filesystem::path AssetDir;
		std::filesystem::path CoreDir;

		CommandLineArguments  CommandLineArgs;
		ApplicationFlagBits   Flags{};
		KeyCode               FullscreenKey = KeyCode::F11;

		RHI                   GraphicsAPI = Renderer::GetPreferredRHI();
		SampleCount           Samples     = SampleCount::SampleCount8;

		uint32_t              Width = 1280, Height = 720;
		bool                  VSync = true;

		bool HasFlags(ApplicationFlagBits flags) const { return static_cast<uint32_t>(Flags) & static_cast<uint32_t>(flags); }
	};

	class Application : public Singleton<Application>
	{
	public:
		Application(const ApplicationSpecification& spec = {});
		virtual ~Application();

		void Run();
		void Stop();
		void Crash(std::exception& e);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		template<typename T, typename... Args>
		void PushLayer(Args&&... args);

		float GetAppRunningTime() const { return m_TotalTime; }
		std::thread::id GetThreadId() const { return m_AppMainThreadId; }

		const ApplicationSpecification& GetSpecification() const { return m_Spec; }
		ApplicationSpecification& GetSpecification() { return m_Spec; }

		Window* GetWindow() const { return m_Window.get(); }
		AudioOutputDevice* GetAudioOutputDevice() const { return m_AudioOutputDevice.get(); }

		AudioOutputDevice* OpenAudioOutputDevice(const char* deviceName = nullptr);

	protected:
		virtual void OnEvent(Event& e) {}
		virtual void OnRender() {}
		virtual void OnUpdate(Timestep elapsedTime) {}
		virtual void OnCrash(std::exception& e) {}

		bool DefaultEventHandler(Event& e);

	private:
		void OnEventInternal(Event& e);
		void OnKeyPressed(KeyPressedEvent& e);

	private:
		ApplicationSpecification m_Spec;
		Scope<Window> m_Window;
		Scope<AudioOutputDevice> m_AudioOutputDevice;

		ImGuiLayer* m_ImGuiLayer = nullptr;

		std::thread::id m_AppMainThreadId;
		std::vector<Layer*> m_Layers;

		float m_TotalTime = 0.0f;
		bool m_Minimized = false;
		bool m_Running = false;
	};

	template<typename T, typename ...Args>
	inline void Application::PushLayer(Args && ...args)
	{
		m_Layers.push_back(new T(std::forward<Args>(args)...));
	}
}

#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Event/Event.h"
#include "Quark/Event/WindowEvent.h"

#include "Quark/Renderer/Renderer.h"
#include "Quark/Renderer/Cubemap.h"

#include "Scene.h"

namespace Quark {

	class PresentableScene : public Scene
	{
	public:
		void OnRender();
		void OnEvent(Event& e);

		Entity CreatePrimaryCamera();
		void SetPrimaryCamera(Entity cameraEntity);
		void SetEnvironment(std::string_view filepath);
		bool HasPrimaryCamera() const { return (bool)m_PrimaryCameraEntity; }

	private:
		void OnWindowResized(WindowResizedEvent& e);
		void NewEnvironment(std::string_view filepath);

	private:
		struct EnvironmentData
		{
			Scope<Shader> SkyboxShader;
			Scope<Shader> IrradianceShader;
			Scope<Shader> EquirectangleToCubemapShader;

			Scope<RenderPass>  RenderPass;
			Scope<FramebufferAttachment> ColorAttachment;
			Scope<Framebuffer> Framebuffer;

			Scope<Pipeline> EnvironmentMapPipeline;
			Scope<Pipeline> IrradiancePipeline;
			Scope<Pipeline> SkyboxPipeline;

			Mesh CubemapBox;

			Scope<Cubemap> Environment;
			Scope<Cubemap> Irradiance;
		};

		struct SceneData
		{
			Scope<EnvironmentData> Env;
		};

		SceneData m_Data;
		Entity m_PrimaryCameraEntity;
	};
}

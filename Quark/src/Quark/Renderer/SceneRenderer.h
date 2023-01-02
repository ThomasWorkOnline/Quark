#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Scene.h"

#include "Cubemap.h"
#include "Renderer.h"

namespace Quark {

	class SceneRenderer
	{
	public:
		SceneRenderer(Scene* scene);

		void OnRender();
		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

		void SetPrimaryCamera(Entity cameraEntity);
		void SetEnvironment(std::string_view filepath);
		bool HasPrimaryCamera() const { return (bool)m_PrimaryCameraEntity; }

	private:
		void NewEnvironment(std::string_view filepath);

	private:
		struct EnvironmentData
		{
			Scope<Shader> SkyboxShader;
			Scope<Shader> IrradianceShader;
			Scope<Shader> EquirectangleToCubemapShader;

			Scope<RenderPass>  RenderPass;
			Scope<Framebuffer> Framebuffer;
			Scope<FramebufferAttachment> EnvironmentBuffer;

			Scope<Pipeline> EnvironmentMapPipeline;
			Scope<Pipeline> IrradiancePipeline;
			Scope<Pipeline> SkyboxPipeline;

			StaticMesh CubemapBox;

			Scope<Cubemap> Environment;
			Scope<Cubemap> Irradiance;
		};

		struct SceneData
		{
			Scope<EnvironmentData> Env;
		};

		SceneData m_Data;

		Scene* m_Scene;
		Entity m_PrimaryCameraEntity;
	};
}

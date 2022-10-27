#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Event/Event.h"
#include "Quark/Event/WindowEvent.h"

#include "Cubemap.h"
#include "Renderer.h"

#include "Quark/Scene/Scene.h"

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

		Scene* m_Scene;
		Entity m_PrimaryCameraEntity;
	};
}

#include "Quark/Scene/Components.h"

namespace Quark {

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Component template specialization
	//

	template<>
	inline void Scene::OnComponentAdded(Entity entity, CameraComponent& cc)
	{
		auto extent = Renderer::GetViewportExtent();
		cc.Camera.Resize(extent.Width, extent.Height);
	}
}

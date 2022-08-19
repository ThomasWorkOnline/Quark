#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Scene.h"

#include "Buffer.h"
#include "Cubemap.h"
#include "Framebuffer.h"
#include "Shader.h"

namespace Quark {

	class PresentableScene : public Scene
	{
	public:
		void OnRender();
		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);
		void SetEnvironment(std::string_view filepath);

	private:
		void NewEnvironment(std::string_view filepath);

	private:
		struct EnvironmentData
		{
			Scope<Shader> SkyboxShader;
			Scope<Shader> IrradianceShader;
			Scope<Shader> EquirectangleToCubemapShader;
			Scope<VertexBuffer> CubemapVertexBuffer;
			Scope<IndexBuffer> CubemapIndexBuffer;
			Scope<Framebuffer> Framebuffer;

			Scope<Cubemap> Environment;
			Scope<Cubemap> Irradiance;
		};

		struct SceneData
		{
			Scope<EnvironmentData> Env;
		};

		SceneData m_Data;
	};
}

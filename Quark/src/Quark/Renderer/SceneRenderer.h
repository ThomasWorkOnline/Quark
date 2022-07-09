#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Scene/Scene.h"

#include "Buffer.h"
#include "Cubemap.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Shader.h"
#include "UniformBuffer.h"

namespace Quark {

	class SceneRenderer
	{
	public:
		SceneRenderer();
		SceneRenderer(const Ref<Scene>& scene);
		~SceneRenderer();

		void SetContext(const Ref<Scene>& scene);
		void SetEnvironment(std::string_view filepath);

		void OnRender();
		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

	private:
		void GeometryPass();

		void Initialize();
		void NewEnvironment(std::string_view environmentFilepath);

	private:
		struct EnvironmentData
		{
			Ref<Shader> SkyboxShader;
			Ref<Shader> IrradianceShader;
			Ref<Shader> EquirectangleToCubemapShader;
			Ref<VertexBuffer> CubemapVertexBuffer;
			Ref<IndexBuffer> CubemapIndexBuffer;
			Ref<Framebuffer> Framebuffer;

			Ref<Cubemap> Environment;
			Ref<Cubemap> Irradiance;
		};

		struct SceneData
		{
			// Assure std140 layout
			struct CameraUniformBufferData
			{
				Mat4f View = Mat4(1.0f);
				Mat4f Projection = Mat4(1.0f);
			};

			Scope<EnvironmentData> Env;
			CameraUniformBufferData CameraBufferData;

			Ref<Shader> VertexShader;
			Ref<Shader> FragmentShader;

			Ref<Pipeline> GraphicsPipeline;
			Ref<RenderPass> GeometryPass;

			Ref<VertexBuffer> VertexBuffer;
			Ref<IndexBuffer> IndexBuffer;
		};

		SceneData m_Data;
		Ref<Scene> m_Scene;
		Vec2i m_ViewportSize{};
	};
}

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
		~SceneRenderer();

		void OnRender();
		void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight);

		void SetContext(Ref<Scene> scene);
		void SetEnvironment(std::string_view filepath);

	private:
		void GeometryPass();
		void Initialize();
		void NewEnvironment(std::string_view filepath);

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

			Ref<VertexBuffer> VertexBuffer;
			Ref<IndexBuffer> IndexBuffer;

			Ref<Shader> Shader;
			Ref<RenderPass> GeometryPass;
			Ref<Pipeline> GraphicsPipeline;
		};

		Ref<Scene> m_Scene;
		SceneData m_Data;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};
}

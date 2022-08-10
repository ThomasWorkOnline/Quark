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
			// Assure std140 layout
			struct CameraUniformBufferData
			{
				Mat4f View = Mat4(1.0f);
				Mat4f Projection = Mat4(1.0f);
			};

			Scope<EnvironmentData>  Env;
			CameraUniformBufferData CameraBufferData;

			Scope<VertexBuffer> VertexBuffer;
			Scope<IndexBuffer>  IndexBuffer;

			Scope<Shader>   Shader;
			Scope<Pipeline> GraphicsPipeline;
		};

		Ref<Scene> m_Scene;
		SceneData m_Data;
	};
}

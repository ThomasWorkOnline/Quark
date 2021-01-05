#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "Mesh.h"

namespace Entropy {

	class Renderer
	{
	public:
		static void Init();
		static void Dispose();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera);
		static void EndScene();
		static void Flush();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::vec3 CameraPosition;
		};

		static SceneData s_SceneData;

		static void StartBatch();
		static void NextBatch();
	};
}

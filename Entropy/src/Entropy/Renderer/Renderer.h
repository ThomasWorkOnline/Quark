#pragma once

#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "SubTexture.h"
#include "Texture.h"

namespace Entropy {

	struct RenderStats
	{
		uint32_t DrawCalls;
	};

	class Renderer
	{
	public:
		static void Init();
		static void Dispose();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitSprite(const Ref<Texture2D>& texture, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitSprite(const SubTexture2D& subTexture, const glm::mat4& transform = glm::mat4(1.0f));

		static const RenderStats& GetStats() { return s_Stats; }

	private:
		static void StartBatch();
		static void PushBatch();

		struct SceneData
		{
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
		};

		static SceneData s_SceneData;
		static RenderStats s_Stats;
	};
}

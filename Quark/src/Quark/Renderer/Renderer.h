#pragma once

#include "Font.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "SubTexture.h"
#include "Texture.h"

#include "../UI/Text.h"

#include "../Scene/Components.h"
#include "../Scene/Entity.h"
#include "../Scene/Scene.h"

namespace Quark {

	struct RendererStats
	{
		uint32_t DrawCalls;
		uint32_t QuadsDrawn;
	};

	class Renderer
	{
	public:
		Renderer() = delete;
		Renderer operator= (const Renderer& other) = delete;

		static void Initialize(uint32_t width, uint32_t height);
		static void Dispose();

		static void OnWindowResize(uint32_t width, uint32_t height);

		/// <summary>
		/// Recieves the camera tranformation in world space, not the view matrix.
		/// This function applies the inverse transformation for the position.
		/// Make sure to call `EndScene` to flush all queued objects.
		/// </summary>
		/// <param name="cameraProjection">Camera projection matrix</param>
		/// <param name="cameraTransform">Camera transform in world space</param>
		static void BeginScene(const glm::mat4& cameraProjection, const Transform3DComponent& cameraTransform);

		/// <summary>
		/// Recieves the camera view matrix.
		/// No extra transformation is applied to the view matrix.
		/// Make sure to call `EndScene` to flush all queued objects.
		/// </summary>
		/// <param name="cameraProjection">Camera projection matrix</param>
		/// <param name="cameraView">Camera view matrix</param>
		static void BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitSprite(const Ref<Texture2D>& texture, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitSprite(const SubTexture2D& subTexture, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitSprite(const glm::vec4& color, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitText(const Text& text, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitText(const Ref<Font>& font, const std::string& text, const glm::vec4& color, const glm::vec2& size, const glm::vec2& origin = glm::vec2(0.0f, 0.0f), const glm::mat4& transform = glm::mat4(1.0f));

		static const RendererStats& GetStats() { return s_Stats; }

	private:
		static void StartBatch();
		static void PushBatch();

		static void SetupQuadRenderer();

		struct SceneData
		{
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
		};

		static SceneData s_SceneData;
		static RendererStats s_Stats;

		static uint32_t s_ViewportWidth, s_ViewportHeight;
	};
}

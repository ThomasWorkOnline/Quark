#pragma once

#include "Quark/Core/Core.h"
#include "Quark/UI/Text.h"

#include "Quark/Scene/Components.h"
#include "Quark/Scene/Entity.h"
#include "Quark/Scene/Scene.h"

#include "Font.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "OrthographicCamera.h"
#include "Shader.h"
#include "SubTexture.h"
#include "Texture.h"
#include "TextureArray.h"

namespace Quark {

	struct Renderer2DStats
	{
		uint32_t DrawCalls;
		uint32_t QuadsDrawn;
		uint32_t LinesDrawn;
	};

	class Renderer2D
	{
	public:
		Renderer2D() = delete;
		Renderer2D& operator=(const Renderer2D& other) = delete;

		static void Initialize();
		static void Dispose();

		/// <summary>
		/// Receives the camera tranformation in world space, not the view matrix.
		/// This function applies the inverse transformation for the position.
		/// Make sure to call `EndScene` to flush all queued objects.
		/// </summary>
		/// <param name="cameraProjection">Camera projection matrix</param>
		/// <param name="cameraTransform">Camera transform in world space</param>
		static void BeginScene(const glm::mat4& cameraProjection, const Transform3DComponent& cameraTransform);

		/// <summary>
		/// Receives the camera view matrix.
		/// No extra transformation is applied to the view matrix.
		/// Make sure to call `EndScene` to flush all queued objects.
		/// </summary>
		/// <param name="cameraProjection">Camera projection matrix</param>
		/// <param name="cameraView">Camera view matrix</param>
		static void BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView);
		static void EndScene();

		static void DrawSprite(const Ref<Texture2D>& texture, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawSprite(const SubTexture2D& subTexture, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawSprite(const glm::vec4& color, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawText(const Text& text);
		static void DrawText(const Ref<Font>& font, std::string_view text, const glm::vec4& color, const glm::ivec2& origin = glm::ivec2(0.0f, 0.0f));
		static void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& beginColor, const glm::vec4& endColor);

		static const Renderer2DStats& GetStats() { return s_Stats; }

	private:
		struct Renderer2DSetupData;

		static void StartBatch();
		static void PushBatch();
		static void ResetStats();

		static void SetupQuadRenderer(Renderer2DSetupData& setupData);
		static void SetupFontRenderer(Renderer2DSetupData& setupData);
		static void SetupLineRenderer();

		static Renderer2DStats s_Stats;
	};
}

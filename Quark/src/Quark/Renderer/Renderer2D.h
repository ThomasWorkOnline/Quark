#pragma once

#include "Quark/Core/Core.h"
#include "Quark/UI/Text.h"
#include "Quark/UI/TextInput.h"

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

		static void DrawSprite(const Ref<Texture2D>& texture, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawSprite(const SubTexture2D& subTexture, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawSprite(const glm::vec4& color, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& beginColor, const glm::vec4& endColor);
		static void DrawText(std::string_view text, const TextRenderTraits& traits);
		static void DrawText(const Text& text);
		static void DrawTextInput(const TextInput& input);

		static const Renderer2DStats& GetStats() { return s_Stats; }

	private:
		static void Initialize();
		static void Dispose();

		static void BeginScene();
		static void EndScene();

		static void StartBatch();
		static void PushBatch();
		static void ResetStats();

		struct Renderer2DSetupData;
		static void SetupQuadRenderer(Renderer2DSetupData& setupData);
		static void SetupFontRenderer(Renderer2DSetupData& setupData);
		static void SetupLineRenderer();

		static Renderer2DStats s_Stats;
		friend class Renderer;
	};
}

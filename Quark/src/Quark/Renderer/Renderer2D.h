#pragma once

#include "Quark/Core/Core.h"
#include "Quark/UI/Text.h"
#include "Quark/UI/TextInput.h"

#include "Quark/Scene/Components.h"
#include "Quark/Scene/Entity.h"
#include "Quark/Scene/Scene.h"

#include "Quark/Renderer/Renderer.h"

#include "Camera.h"
#include "Font.h"
#include "SubTexture.h"
#include "Texture.h"

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
		static void BeginScene(const Camera& camera, const Transform3DComponent& cameraTransform);
		static void BeginScene(const Mat4f& cameraProjection, const Mat4f& cameraView);
		static void EndScene();

		static void DrawSprite(const Texture* texture, const Vec4f& tint = Vec4(1.0f), const Mat4f& transform = Mat4f(1.0f));
		static void DrawSprite(const Texture* texture, float aspectRatio, const Vec4f& tint = Vec4(1.0f));
		static void DrawSprite(const Texture* texture, const Vec2f* texCoords, const Vec4f& tint = Vec4(1.0f), const Mat4f& transform = Mat4f(1.0f));
		static void DrawSprite(const SubTexture2D& subTexture, const Vec4f& tint = Vec4(1.0f), const Mat4f& transform = Mat4f(1.0f));
		static void DrawSprite(const Vec4f& color, const Mat4f& transform = Mat4f(1.0f));

		static void DrawLine(const Vec3f& p1, const Vec3f& p2, const Vec4f& beginColor, const Vec4f& endColor);

		static void DrawText(std::string_view text, const Font* font, const Vec4f& color = Vec4f(1.0f), const Mat4f& transform = Mat4f(1.0f));
		static void DrawText(const Text& text, const Mat4f& transform = Mat4f(1.0f));

		static const Renderer2DStats& GetStats() { return s_Stats; }

	private:
		static void StartBatch();
		static void PushBatch();
		static void ResetStats();

		static void Initialize();
		static void Dispose();

		friend class Application;
		static Renderer2DStats s_Stats;
	};
}

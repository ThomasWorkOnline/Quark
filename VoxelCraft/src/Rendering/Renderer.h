#pragma once

#include <Quark.h>

#include "../World/Chunk.h"
#include "../World/WorldMap.h"

namespace VoxelCraft {

	struct RendererStats
	{
		uint32_t DrawCalls = 0;
	};

	class Renderer
	{
	public:
		Renderer() = delete;
		Renderer operator= (const Renderer& other) = delete;

		static void Initialize();
		static void Shutdown();

		static void BeginScene(const glm::mat4& cameraProjection, const Quark::Transform3DComponent& cameraTransformNoPosition, const Position3D& cameraPosition);
		static void EndScene();

		static void SubmitMap(const WorldMap& map);
		static void RenderUIScene(uint32_t width, uint32_t height);

		// Debug
		static void SwitchShader();
		static void ViewUnloadedChunks(bool enabled) { s_ViewUnloadedChunks = enabled; }

		static const RendererStats& GetStats() { return s_Stats; }

	private:
		static Quark::Ref<Quark::Shader> s_ActiveShader;
		static Quark::Ref<Quark::Texture2D> s_Texture;

		static void RenderUnloadedChunks(const WorldMap& map);
		static void RenderCrosshair();
		static void RenderChunk(const Chunk* chunk);
		
		static void ResetStats();

		static bool s_ViewUnloadedChunks;

		static RendererStats s_Stats;
	};
}

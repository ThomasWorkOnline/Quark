#include "Renderer.h"

#include "../Game/Resources.h"

namespace VoxelCraft {

#define DRAW_CHUNK_ALL_STATUS 1

	Quark::Ref<Quark::Shader> Renderer::s_ActiveShader;
	Quark::Ref<Quark::Texture2D> Renderer::s_Texture;

	bool Renderer::s_ViewUnloadedChunks = false;

	RendererStats Renderer::s_Stats;

	// Debug
	static Quark::Ref<Quark::Shader> s_Shader;
	static const Quark::BufferLayout s_Layout = {
		{ Quark::ShaderDataType::Float3, "a_Position" },
		{ Quark::ShaderDataType::Float3, "a_Normal"   },
		{ Quark::ShaderDataType::Float2, "a_TexCoord" }
	};
	static Quark::Mesh s_Mesh;

	static Quark::Transform3DComponent s_Transform;

	void Renderer::Initialize()
	{
		s_ActiveShader = Resources::GetShader("default");
		s_Texture = Resources::GetTexture();

		s_Shader = Quark::Shader::Create("assets/shaders/default3D.glsl");
		s_Mesh = { s_Layout, "assets/models/arrow.obj" };

		s_Transform.Position = { 0.5f, 80.f, 0.5f };
		s_Transform.Scale = { 0.4f, 0.2f, 0.2f };
		s_Transform.Orientation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::BeginScene(const glm::mat4& cameraProjection, const Quark::Transform3DComponent& cameraTransformNoPosition, const Position3D& cameraPosition)
	{
		Quark::Renderer::BeginScene(cameraProjection, cameraTransformNoPosition);

		s_ActiveShader->Attach();
		s_ActiveShader->SetDouble3("u_CameraPosition", cameraPosition);

		s_Shader->Attach();
		s_Shader->SetDouble3("u_CameraPosition", cameraPosition);
	}

	void Renderer::EndScene()
	{
		Quark::Renderer::EndScene();

		ResetStats();
	}

	void Renderer::SubmitMap(const WorldMap& map)
	{
		map.Foreach([](const Chunk* data)
			{
				if (data->LoadStatus == Chunk::LoadStatus::Loaded)
					Renderer::RenderChunk(data);
			});

		if (s_ViewUnloadedChunks)
			RenderUnloadedChunks(map);
	}

	void Renderer::RenderUnloadedChunks(const WorldMap& map)
	{
		map.Foreach([](const Chunk* data)
			{
				if (data->LoadStatus != Chunk::LoadStatus::Loaded)
				{
					auto&& position = IntPosition2D(data->ID.Coord);
					position.x *= SubChunkSpecification::Width;
					position.y *= SubChunkSpecification::Depth;

					s_Transform.Position = { position.x + 8, 80, position.y + 8 };

					Quark::Renderer::Submit(s_Shader, s_Mesh.GetVertexArray(), s_Transform);
					s_Stats.DrawCalls++;
				}
			});
	}

	void Renderer::RenderUIScene(uint32_t width, uint32_t height)
	{
		// Draw UI
		float aspectRatio = static_cast<float>(width) / height;
		Quark::Renderer::BeginScene(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, aspectRatio, 1.0f)), glm::mat4(1.0f));

		RenderCrosshair();

		Quark::Renderer::EndScene();
	}

	void Renderer::SwitchShader()
	{
		if (s_ActiveShader->GetName() == "default")
		{
			s_ActiveShader = Resources::GetShader("debugMesh");
		}
		else
		{
			s_ActiveShader = Resources::GetShader("default");
		}
	}

	void Renderer::RenderChunk(const Chunk* chunk)
	{
		// Reverse rendering order, draw top chunks first
		for (auto it = chunk->rbegin(); it != chunk->rend(); it++)
		{
			if (!it->Mesh.Empty())
			{
				auto&& position = IntPosition2D(chunk->ID.Coord);
				position.x *= SubChunkSpecification::Width;
				position.y *= SubChunkSpecification::Depth;

				s_ActiveShader->Attach();
				s_ActiveShader->SetInt3("u_ModelOffset", { position.x, 0, position.y });

				Quark::Renderer::Submit(s_ActiveShader, s_Texture, it->Mesh.GetVertexArray());
				s_Stats.DrawCalls++;
			}
		}
	}

	void Renderer::ResetStats()
	{
		s_Stats.DrawCalls = 0;
	}

	void Renderer::RenderCrosshair()
	{
		static const auto& shader = Resources::GetShader("crosshair");
		static const auto& vao = Resources::GetCrosshairVertexArray();

		shader->Attach();
		shader->SetFloat4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

		Quark::Renderer::Submit(shader, vao);
		s_Stats.DrawCalls++;
	}
}

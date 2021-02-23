#include "Renderer.h"

#include "../Core/Core.h"

#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../Scene/Components.h"

#include "RenderingAPI.h"

namespace Entropy {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
		float TexIndex;
	};

	struct SpriteRendererData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		uint32_t MaxTextureSlots;

		Vertex* VertexPtr = nullptr;
		Vertex* Vertices  = nullptr;

		uint32_t IndexCount = 0;

		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;

		Ref<Shader> SpriteShader;
		Ref<Texture2D>* Textures;
		Ref<Texture2D> DefaultTexture;
		uint32_t TextureSlotIndex = 1; // Next texture slot to be attached

		glm::vec4 SpriteVertexPositions[4];
	};

	Renderer::SceneData Renderer::s_SceneData;
	RenderStats Renderer::s_Stats;
	SpriteRendererData s_Data;
	
	void Renderer::Init()
	{
		RenderCommand::Init();

		s_Data.VertexArray  = VertexArray::Create();

		s_Data.VertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(Vertex));
		s_Data.VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal"   },
			{ ShaderDataType::Float,  "a_TexIndex" }
			});
		s_Data.VertexArray->AddVertexBuffer(s_Data.VertexBuffer);

		// Indices
		uint32_t* indices = new uint32_t[s_Data.MaxVertices * 6];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxVertices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 3;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 0;
			indices[i + 4] = offset + 2;
			indices[i + 5] = offset + 1;

			offset += 4;
		}

		s_Data.IndexBuffer = IndexBuffer::Create(indices, s_Data.MaxIndices);
		s_Data.VertexArray->SetIndexBuffer(s_Data.IndexBuffer);
		delete[] indices;

		// Vertices
		s_Data.Vertices = new Vertex[s_Data.MaxVertices];
		memset(s_Data.Vertices, 0, s_Data.MaxVertices);

		s_Data.SpriteVertexPositions[0] = { -0.5f, -0.5f,  0.0f,  1.0f };
		s_Data.SpriteVertexPositions[1] = {  0.5f, -0.5f,  0.0f,  1.0f };
		s_Data.SpriteVertexPositions[2] = {  0.5f,  0.5f,  0.0f,  1.0f };
		s_Data.SpriteVertexPositions[3] = { -0.5f,  0.5f,  0.0f,  1.0f };

		// Textures
		s_Data.MaxTextureSlots = RenderCommand::GetTextureSlotsCount();
		s_Data.Textures = new Ref<Texture2D>[s_Data.MaxTextureSlots];

		uint32_t textureColor = 0xffffffff;
		TextureSpecification spec = { 1, 1, 1, TextureDataFormat::RGBA8,
			TextureFilteringFormat::Nearest, TextureTilingFormat::Repeat
		};
		s_Data.DefaultTexture = Texture2D::Create(spec);
		s_Data.DefaultTexture->SetData(&textureColor, sizeof(uint32_t));

		int32_t* samplers = new int32_t[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.Textures[0] = s_Data.DefaultTexture;

		s_Data.SpriteShader = Shader::Create("assets/shaders/defaultTexture.glsl");
		s_Data.SpriteShader->Attach();
		s_Data.SpriteShader->SetIntArray("u_Samplers", samplers, s_Data.MaxTextureSlots);
		delete[] samplers;
	}

	void Renderer::Dispose()
	{
		delete[] s_Data.Vertices;
		delete[] s_Data.Textures;
	}

	void Renderer::BeginScene(Entity cameraEntity)
	{
		auto& transform = cameraEntity.GetComponent<TransformComponent>();
		auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

		transform.Orientation = glm::normalize(transform.Orientation);
		glm::mat4 rotate = glm::toMat4(transform.Orientation);
		glm::mat4 view = glm::translate(rotate, -transform.Position);

		s_SceneData.ProjectionMatrix = camera.GetProjectionMatrix();
		s_SceneData.ViewMatrix = view;

		StartBatch();

		s_Stats.DrawCalls = 0;
	}

	void Renderer::BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView)
	{
		s_SceneData.ProjectionMatrix = cameraProjection;
		s_SceneData.ViewMatrix = cameraView;

		StartBatch();

		s_Stats.DrawCalls = 0;
	}

	void Renderer::EndScene()
	{
		PushBatch();
	}

	void Renderer::StartBatch()
	{
		s_Data.VertexPtr = s_Data.Vertices;
		s_Data.IndexCount = 0;
		s_Data.TextureSlotIndex = 1; // 0 is reserved
	}

	void Renderer::PushBatch()
	{
		if (s_Data.VertexPtr == s_Data.Vertices)
			return;

		size_t size = ((uint8_t*)s_Data.VertexPtr - (uint8_t*)s_Data.Vertices);
		s_Data.VertexBuffer->SetData(s_Data.Vertices, size);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.Textures[i]->Attach(i);

		s_Data.SpriteShader->Attach();
		s_Data.SpriteShader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		s_Data.SpriteShader->SetMat4("u_View", s_SceneData.ViewMatrix);

		s_Data.VertexArray->Attach();
		RenderCommand::Draw(s_Data.VertexArray, s_Data.IndexCount);

		s_Stats.DrawCalls++;
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		if (va)
		{
			shader->Attach();
			shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
			shader->SetMat4("u_View", s_SceneData.ViewMatrix);
			shader->SetMat4("u_Model", transform);

			texture->Attach(0);

			va->Attach();
			RenderCommand::Draw(va);
		}
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		if (va)
		{
			shader->Attach();
			shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
			shader->SetMat4("u_View", s_SceneData.ViewMatrix);
			shader->SetMat4("u_Model", transform);

			va->Attach();
			RenderCommand::Draw(va);
		}
	}

	void Renderer::SubmitSprite(const Ref<Texture2D>& texture, const glm::mat4& transform)
	{
		if (s_Data.IndexCount >= s_Data.MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Check if texture exists in samplers
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.Textures[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0.0f)
		{
			// If not enough space to attach new texture
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.VertexPtr->Position = transform * s_Data.SpriteVertexPositions[i];
			s_Data.VertexPtr->TexCoord = textureCoords[i];
			s_Data.VertexPtr->Normal   = { 0.0f, 0.0f, 1.0f };
			s_Data.VertexPtr->TexIndex = textureIndex;

			s_Data.VertexPtr++;
		}

		s_Data.IndexCount += 6;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}

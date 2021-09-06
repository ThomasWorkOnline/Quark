#include "Renderer.h"

#include "../Core/Core.h"
#include "../Debug/Monitoring.h"

#include "RenderingAPI.h"

#include <GL/glew.h>

namespace Quark {

	static constexpr glm::vec4 s_SpriteVertexPositions[] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	struct SpriteVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
		glm::vec4 Color;
		float TexIndex;
	};

	struct SpriteRendererData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		uint32_t MaxTextureSlots = 0;

		SpriteVertex* VertexPtr = nullptr;
		SpriteVertex* Vertices  = nullptr;

		uint32_t IndexCount = 0;

		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;

		Ref<Shader> SpriteShader;
		Ref<Texture2D>* Textures = nullptr;
		Ref<Texture2D> DefaultTexture;
		uint32_t TextureSlotIndex = 1; // Next texture slot to be attached
	};

	Renderer::SceneData Renderer::s_SceneData;
	RendererStats Renderer::s_Stats;
	static SpriteRendererData s_Data;
	
	void Renderer::Initialize()
	{
		QK_TIME_SCOPE_DEBUG(Renderer::Initialize);

		RenderCommand::Init();

		{
			s_Data.VertexArray = VertexArray::Create();

			s_Data.VertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(SpriteVertex));
			s_Data.VertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal"   },
				{ ShaderDataType::Float4, "a_Color"    },
				{ ShaderDataType::Float,  "a_TexIndex" }
				});
			s_Data.VertexArray->AddVertexBuffer(s_Data.VertexBuffer);

			// Indices
			uint32_t* indices = new uint32_t[s_Data.MaxIndices];

			uint32_t offset = 0;
			for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
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
			s_Data.Vertices = new SpriteVertex[s_Data.MaxVertices];
			memset(s_Data.Vertices, 0, s_Data.MaxVertices);

			// Textures
			s_Data.MaxTextureSlots = RenderCommand::GetTextureSlotsCount();
			s_Data.Textures = new Ref<Texture2D>[s_Data.MaxTextureSlots];

			uint32_t textureColor = 0xffffffff;
			TextureSpecification spec = { 1, 1, 1, TextureDataFormat::RGBA8,
				TextureFilteringFormat::Nearest, TextureTilingFormat::Repeat
			};
			s_Data.DefaultTexture = Texture2D::Create(spec);
			s_Data.DefaultTexture->SetData(&textureColor, sizeof(uint32_t));

			s_Data.Textures[0] = s_Data.DefaultTexture;

			const char* vertexSource = R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec2 a_TexCoord;
				layout(location = 2) in vec3 a_Normal;
				layout(location = 3) in vec4 a_Color;
				layout(location = 4) in float a_TexIndex;

				uniform mat4 u_View;
				uniform mat4 u_Projection;

				out vec3 v_Position;
				out vec2 v_TexCoord;
				out vec3 v_Normal;
				out vec4 v_Color;
				flat out float v_TexIndex;

				void main()
				{
					vec4 position = vec4(a_Position, 1.0);

					gl_Position = u_Projection * u_View * position;

					v_Position = position.xyz;
					v_TexCoord = a_TexCoord;
					v_Normal = a_Normal;
					v_Color = a_Color;
					v_TexIndex = a_TexIndex;
				}
			)";

			const char* fragmentSource = R"(
				#version 330 core

				uniform sampler2D u_Samplers[32];

				in vec3 v_Position;
				in vec2 v_TexCoord;
				in vec3 v_Normal;
				in vec4 v_Color;
				flat in float v_TexIndex;

				void main()
				{
					vec4 color;

					if (v_TexIndex == -1.0)
						color = v_Color;
					else
						color = texture(u_Samplers[int(v_TexIndex)], v_TexCoord);

					if (color.a < 1)
						discard;
	
					gl_FragColor = color;
				}
			)";

			int32_t* samplers = new int32_t[s_Data.MaxTextureSlots];
			for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
				samplers[i] = i;

			s_Data.SpriteShader = Shader::Create("defaultSprite", vertexSource, fragmentSource);
			s_Data.SpriteShader->Attach();
			s_Data.SpriteShader->SetIntArray("u_Samplers", samplers, s_Data.MaxTextureSlots);
			delete[] samplers;
		}
	}

	void Renderer::Dispose()
	{
		delete[] s_Data.Vertices;
		delete[] s_Data.Textures;
	}

	void Renderer::BeginScene(const glm::mat4& cameraProjection, const Transform3DComponent& cameraTransform)
	{
		glm::mat4 rotate = glm::toMat4(cameraTransform.Orientation);
		glm::mat4 view = glm::translate(rotate, (glm::vec3)-cameraTransform.Position);
		
		BeginScene(cameraProjection, view);
	}

	void Renderer::BeginScene(const glm::mat4& cameraProjection, const glm::mat4& cameraView)
	{
		s_SceneData.ProjectionMatrix = cameraProjection;
		s_SceneData.ViewMatrix = cameraView;

		StartBatch();

		s_Stats.DrawCalls = 0;
		s_Stats.QuadsDrawn = 0;
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
		if (s_Data.IndexCount == 0)
			return;

		size_t size = ((uint8_t*)s_Data.VertexPtr - (uint8_t*)s_Data.Vertices);
		s_Data.VertexBuffer->SetData(s_Data.Vertices, size);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.Textures[i]->Attach(i);

		s_Data.SpriteShader->Attach();
		s_Data.SpriteShader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		s_Data.SpriteShader->SetMat4("u_View", s_SceneData.ViewMatrix);

		s_Data.VertexArray->Attach();
		RenderCommand::DrawIndexed(s_Data.VertexArray, s_Data.IndexCount);

		s_Stats.DrawCalls++;
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Texture2D>& texture, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Attach();
		shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		shader->SetMat4("u_View", s_SceneData.ViewMatrix);
		shader->SetMat4("u_Model", transform);

		texture->Attach();

		va->Attach();
		RenderCommand::DrawIndexed(va);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Attach();
		shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		shader->SetMat4("u_View", s_SceneData.ViewMatrix);
		shader->SetMat4("u_Model", transform);

		s_Data.DefaultTexture->Attach(0);
		s_Data.DefaultTexture->Attach(1);

		va->Attach();
		RenderCommand::DrawIndexed(va);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<Framebuffer>& framebuffer, const Ref<VertexArray>& va, const glm::mat4& transform)
	{
		shader->Attach();
		shader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
		shader->SetMat4("u_View", s_SceneData.ViewMatrix);
		shader->SetMat4("u_Model", transform);

		framebuffer->AttachColorAttachment(0);
		framebuffer->AttachColorAttachment(1);

		va->Attach();
		RenderCommand::DrawIndexed(va);
	}

	void Renderer::SubmitSprite(const Ref<Texture2D>& texture, const glm::mat4& transform)
	{
		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		SubmitSprite(texture, textureCoords, transform);
	}

	void Renderer::SubmitSprite(const SubTexture2D& subTexture, const glm::mat4& transform)
	{
		SubmitSprite(subTexture.GetTexture(), subTexture.GetCoords(), transform);
	}

	void Renderer::SubmitSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform)
	{
		// Check if buffer is full
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

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.VertexPtr->Position = transform * s_SpriteVertexPositions[i];
			s_Data.VertexPtr->TexCoord = texCoords[i];
			s_Data.VertexPtr->Normal   = { 0.0f, 0.0f, 1.0f };
			s_Data.VertexPtr->Color    = { 0.0f, 0.0f, 0.0f, 1.0f };
			s_Data.VertexPtr->TexIndex = textureIndex;

			s_Data.VertexPtr++;
		}

		s_Data.IndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer::SubmitSprite(const glm::vec4& color, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data.IndexCount >= s_Data.MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

#if 0
		// Viewport culling
		glm::mat4 mvp = s_SceneData.ProjectionMatrix * s_SceneData.ViewMatrix * transform;
		glm::vec3 bottomLeft = mvp * spriteVertexPositions[0];
		glm::vec3 topRight = mvp * spriteVertexPositions[2];

		if (topRight.x < -1.0f)
			return;

		if (bottomLeft.x > 1.0f)
			return;

		if (topRight.y < -1.0f)
			return;

		if (bottomLeft.y > 1.0f)
			return;
#endif

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.VertexPtr->Position = transform * s_SpriteVertexPositions[i];
			s_Data.VertexPtr->TexCoord = { 0.0f, 0.0f };
			s_Data.VertexPtr->Normal   = { 0.0f, 0.0f, 1.0f };
			s_Data.VertexPtr->Color    = color;
			s_Data.VertexPtr->TexIndex = -1.0f;

			s_Data.VertexPtr++;
		}

		s_Data.IndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}

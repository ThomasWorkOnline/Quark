#include "Renderer.h"

#include "../Core/Core.h"
#include "../Core/Application.h"
#include "../Debug/Monitoring.h"

#include "RenderingAPI.h"

namespace Quark {

	static constexpr glm::vec4 s_SpriteVertexPositions[] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	enum class RenderMode : uint32_t
	{
		RenderTexture		= 0,
		RenderFont			= 1,
		RenderColor			= 2
	};

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		int32_t TexIndex;
		RenderMode Mode;
	};

	struct QuadRendererData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		uint32_t MaxTextureSlots = 0;

		QuadVertex* VertexPtr = nullptr;
		QuadVertex* Vertices  = nullptr;

		uint32_t IndexCount = 0;

		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;

		Ref<Shader> SpriteShader;
		Ref<Shader> TextShader;
		Ref<Texture2D>* Textures = nullptr;
		Ref<Font>* Fonts = nullptr;

		Ref<Texture2D> DefaultTexture;
		uint32_t TextureSlotIndex = 1; // Next texture slot to be attached
	};

	Renderer::SceneData Renderer::s_SceneData;
	RendererStats Renderer::s_Stats;
	static QuadRendererData s_Data;

	uint32_t Renderer::s_ViewportWidth = 0;
	uint32_t Renderer::s_ViewportHeight = 0;
	
	void Renderer::Initialize(uint32_t width, uint32_t height)
	{
		QK_TIME_SCOPE_DEBUG(Renderer::Initialize);

		s_ViewportWidth = width; s_ViewportHeight = height;

		RenderCommand::Init();
	}

	void Renderer::InitializeBatchRenderer()
	{
		SetupQuadRenderer();
	}

	void Renderer::SetupQuadRenderer()
	{
		s_Data.VertexArray = VertexArray::Create();

		s_Data.VertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.VertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float4, "a_Color"    },
				{ ShaderDataType::Int,    "a_TexIndex" },
				{ ShaderDataType::Int,    "a_Mode"     }
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
		s_Data.Vertices = new QuadVertex[s_Data.MaxVertices];
		memset(s_Data.Vertices, 0, s_Data.MaxVertices * sizeof(QuadVertex));

		// Textures
		s_Data.MaxTextureSlots = RenderCommand::GetTextureSlotsCount();
		s_Data.Textures = new Ref<Texture2D>[s_Data.MaxTextureSlots];
		s_Data.Fonts = new Ref<Font>[s_Data.MaxTextureSlots];

		uint32_t textureColor = 0xffffffff;
		TextureSpecification spec = { 1, 1, 1, TextureDataFormat::RGBA8,
			TextureFilteringMode::Nearest, TextureFilteringMode::Nearest, TextureTilingMode::Repeat
		};
		s_Data.DefaultTexture = Texture2D::Create(spec);
		s_Data.DefaultTexture->SetData(&textureColor, sizeof(uint32_t));

		s_Data.Textures[0] = s_Data.DefaultTexture;

		const char* spriteVertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			layout(location = 2) in vec4 a_Color;
			layout(location = 3) in int  a_TexIndex;
			layout(location = 4) in int  a_Mode;

			uniform mat4 u_View;
			uniform mat4 u_Projection;

			out vec3 v_Position;
			out vec2 v_TexCoord;
			out vec4 v_Color;
			flat out int v_TexIndex;
			flat out int v_Mode;

			void main()
			{
				vec4 position = vec4(a_Position, 1.0);

				gl_Position = u_Projection * u_View * position;

				v_Position	= position.xyz;
				v_TexCoord	= a_TexCoord;
				v_Color		= a_Color;
				v_TexIndex	= a_TexIndex;
				v_Mode		= a_Mode;
			}
		)";

		const char* spriteFragmentSource = R"(
			#version 330 core

			out vec4 u_FragColor;

			uniform sampler2D u_Samplers[32];

			in vec3 v_Position;
			in vec2 v_TexCoord;
			in vec4 v_Color;
			flat in int v_TexIndex;
			flat in int v_Mode;

			void main()
			{
				switch (v_Mode)
				{
				case 0:
				{
					// Texture
					u_FragColor = texture(u_Samplers[v_TexIndex], v_TexCoord.xy);
					break;
				}
				case 1:
				{
					// Text
					float texture = texture(u_Samplers[v_TexIndex], v_TexCoord.xy, 0).r;
					u_FragColor = v_Color * texture;
					break;
				}
				case 2:
				{
					// Color only
					u_FragColor = v_Color;
					break;
				}
				}
			}
		)";

		int32_t* samplers = new int32_t[s_Data.MaxTextureSlots];
		for (int32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.SpriteShader = Shader::Create("defaultSprite", spriteVertexSource, spriteFragmentSource);

		s_Data.SpriteShader->Attach();
		s_Data.SpriteShader->SetIntArray("u_Samplers", samplers, s_Data.MaxTextureSlots);

		delete[] samplers;
	}

	void Renderer::Dispose()
	{
		delete[] s_Data.Vertices;
		delete[] s_Data.Textures;
		delete[] s_Data.Fonts;
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
		
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.Textures[i].reset();
			s_Data.Fonts[i].reset();
		}

		s_Data.TextureSlotIndex = 1; // 0 is reserved
	}

	void Renderer::PushBatch()
	{
		if (s_Data.IndexCount == 0)
			return;

		size_t size = ((uint8_t*)s_Data.VertexPtr - (uint8_t*)s_Data.Vertices);
		s_Data.VertexBuffer->SetData(s_Data.Vertices, size);

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.Textures[i])
				s_Data.Textures[i]->Attach(i);
			else if (s_Data.Fonts[i])
				s_Data.Fonts[i]->Attach(i);
			else QK_ASSERT(false, "No texture was attached");
		}

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

		//s_Data.DefaultTexture->Attach(0);
		//s_Data.DefaultTexture->Attach(1);

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
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.Textures[i] && *s_Data.Textures[i] == *texture)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0)
		{
			// If not enough space to attach new texture
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data.TextureSlotIndex;
			s_Data.Textures[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.VertexPtr->Position	= transform * s_SpriteVertexPositions[i];
			s_Data.VertexPtr->TexCoord	= texCoords[i];
			s_Data.VertexPtr->Color		= { 0.0f, 0.0f, 0.0f, 1.0f };
			s_Data.VertexPtr->TexIndex	= textureIndex;
			s_Data.VertexPtr->Mode		= RenderMode::RenderTexture;

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
			s_Data.VertexPtr->Position	= transform * s_SpriteVertexPositions[i];
			s_Data.VertexPtr->TexCoord	= { 0.0f, 0.0f };
			s_Data.VertexPtr->Color		= color;
			s_Data.VertexPtr->TexIndex	= 0;
			s_Data.VertexPtr->Mode		= RenderMode::RenderColor;

			s_Data.VertexPtr++;
		}

		s_Data.IndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer::SubmitText(const Text& text, const glm::mat4& transform)
	{
		SubmitText(text.GetFont(), text.GetString(), text.GetColor(), { 1.0f, 1.0f }, text.GetOrigin(), transform);
	}

	void Renderer::SubmitText(const Ref<Font>& font, const std::string& text, const glm::vec4& color, const glm::vec2& size, const glm::vec2& origin, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data.IndexCount >= s_Data.MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.Fonts[i] && *s_Data.Fonts[i] == *font)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0)
		{
			// If not enough space to attach new texture
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data.TextureSlotIndex;
			s_Data.Fonts[s_Data.TextureSlotIndex] = font;
			s_Data.TextureSlotIndex++;
		}

		constexpr float scale = 0.001f;

		int32_t x = -origin.x;
		int32_t y = -origin.y;
		for (uint32_t i = 0; i < text.size(); i++)
		{
			auto& ch = font->GetCharacter(text[i]);

			float xpos = (x + ch.Bearing.x) * scale;
			float ypos = (-y - ch.Bearing.y) * scale;
			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			float tx = ch.TexCoordX;

			x += ch.Advance.x;
			y += ch.Advance.y;

			if (!w || !h)
				continue;

			float atlasWidth = font->GetAtlasWidth();
			float atlasHeight = font->GetAtlasHeight();

			s_Data.VertexPtr->Position	= transform * glm::vec4(xpos + w, -ypos, 0.0f, 1.0f);
			s_Data.VertexPtr->TexCoord	= { tx + ch.Size.x / atlasWidth, 0.0f };
			s_Data.VertexPtr->Color		= color;
			s_Data.VertexPtr->TexIndex	= textureIndex;
			s_Data.VertexPtr->Mode		= RenderMode::RenderFont;
			s_Data.VertexPtr++;

			s_Data.VertexPtr->Position	= transform * glm::vec4(xpos, -ypos, 0.0f, 1.0f);
			s_Data.VertexPtr->TexCoord	= { tx, 0.0f };
			s_Data.VertexPtr->Color		= color;
			s_Data.VertexPtr->TexIndex	= textureIndex;
			s_Data.VertexPtr->Mode		= RenderMode::RenderFont;
			s_Data.VertexPtr++;

			s_Data.VertexPtr->Position	= transform * glm::vec4(xpos, -ypos - h, 0.0f, 1.0f);
			s_Data.VertexPtr->TexCoord	= { tx, ch.Size.y / atlasHeight };
			s_Data.VertexPtr->Color		= color;
			s_Data.VertexPtr->TexIndex	= textureIndex;
			s_Data.VertexPtr->Mode		= RenderMode::RenderFont;
			s_Data.VertexPtr++;

			s_Data.VertexPtr->Position	= transform * glm::vec4(xpos + w, -ypos - h, 0.0f, 1.0f);
			s_Data.VertexPtr->TexCoord	= { tx + ch.Size.x / atlasWidth, ch.Size.y / atlasHeight };
			s_Data.VertexPtr->Color		= color;
			s_Data.VertexPtr->TexIndex	= textureIndex;
			s_Data.VertexPtr->Mode		= RenderMode::RenderFont;
			s_Data.VertexPtr++;

			s_Data.IndexCount += 6;
			s_Stats.QuadsDrawn++;
		}
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		s_ViewportWidth = width; s_ViewportHeight = height;
		RenderCommand::SetViewport(0, 0, width, height);
	}
}

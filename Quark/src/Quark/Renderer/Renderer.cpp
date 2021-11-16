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

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		int32_t TexIndex;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct SetupData
	{
		uint32_t* Indices = nullptr;
		int32_t* Samplers = nullptr;
	};

	struct RendererData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		uint32_t MaxSamplers = 0;

		QuadVertex* QuadVertexPtr = nullptr;
		QuadVertex* QuadVertices  = nullptr;
		uint32_t QuadIndexCount   = 0;
		uint32_t QuadSamplerIndex = 1; // Next texture slot to be attached, 0 is reserved for default texture
		Ref<Shader> QuadShader;
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;

		QuadVertex* FontVertexPtr = nullptr;
		QuadVertex* FontVertices  = nullptr;
		uint32_t FontIndexCount   = 0;
		uint32_t FontSamplerIndex = 0;
		Ref<Shader> FontShader;
		Ref<VertexArray> FontVertexArray;
		Ref<VertexBuffer> FontVertexBuffer;

		LineVertex* LineVertexPtr = nullptr;
		LineVertex* LineVertices  = nullptr;
		Ref<Shader> LineShader;
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;

		Ref<Texture2D> DefaultTexture;
		Ref<Texture2D>* Textures = nullptr;
		Ref<Font>* Fonts = nullptr;
	};

	Renderer::SceneData Renderer::s_SceneData;
	RendererStats Renderer::s_Stats;

	uint32_t Renderer::s_ViewportWidth = 0;
	uint32_t Renderer::s_ViewportHeight = 0;

	static RendererData s_Data;
	static SetupData s_SetupData;
	
	void Renderer::Initialize(uint32_t width, uint32_t height)
	{
		QK_TIME_SCOPE_DEBUG(Renderer::Initialize);

		s_ViewportWidth = width; s_ViewportHeight = height;

		RenderCommand::Init();

		// Samplers
		s_Data.MaxSamplers = RenderCommand::GetTextureSlotsCount();
		s_SetupData.Samplers = new int32_t[s_Data.MaxSamplers];
		for (int32_t i = 0; i < s_Data.MaxSamplers; i++)
			s_SetupData.Samplers[i] = i;

		// Indices
		s_SetupData.Indices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			s_SetupData.Indices[i + 0] = offset + 0;
			s_SetupData.Indices[i + 1] = offset + 3;
			s_SetupData.Indices[i + 2] = offset + 2;
			s_SetupData.Indices[i + 3] = offset + 0;
			s_SetupData.Indices[i + 4] = offset + 2;
			s_SetupData.Indices[i + 5] = offset + 1;

			offset += 4;
		}

		SetupQuadRenderer();
		SetupFontRenderer();
		SetupLineRenderer();

		delete[] s_SetupData.Indices;
		delete[] s_SetupData.Samplers;
	}

	void Renderer::SetupQuadRenderer()
	{
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_Tint"     },
			{ ShaderDataType::Int,    "a_TexIndex" }
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		// Indices
		s_Data.QuadIndexBuffer = IndexBuffer::Create(s_SetupData.Indices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);

		// Vertices
		s_Data.QuadVertices = new QuadVertex[s_Data.MaxVertices];

		// Textures
		s_Data.Textures = new Ref<Texture2D>[s_Data.MaxSamplers];

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
			layout(location = 2) in vec4 a_Tint;
			layout(location = 3) in int  a_TexIndex;

			uniform mat4 u_View;
			uniform mat4 u_Projection;

			out vec3 v_Position;
			out vec2 v_TexCoord;
			out vec4 v_Tint;
			flat out int v_TexIndex;

			void main()
			{
				vec4 position = vec4(a_Position, 1.0);

				gl_Position = u_Projection * u_View * position;

				v_Position	= position.xyz;
				v_TexCoord	= a_TexCoord;
				v_Tint		= a_Tint;
				v_TexIndex	= a_TexIndex;
			}
		)";

		const char* spriteFragmentSource = R"(
			#version 330 core

			out vec4 o_FragColor;

			uniform sampler2D u_Samplers[32];

			in vec3 v_Position;
			in vec2 v_TexCoord;
			in vec4 v_Tint;
			flat in int v_TexIndex;

			void main()
			{
				o_FragColor = texture(u_Samplers[v_TexIndex], v_TexCoord.xy) * v_Tint;
			}
		)";

		s_Data.QuadShader = Shader::Create("defaultSprite", spriteVertexSource, spriteFragmentSource);
		s_Data.QuadShader->Attach();
		s_Data.QuadShader->SetIntArray("u_Samplers", s_SetupData.Samplers, s_Data.MaxSamplers);
	}

	void Renderer::SetupFontRenderer()
	{
		s_Data.FontVertexArray = VertexArray::Create();

		s_Data.FontVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.FontVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_TexIndex" }
		});
		s_Data.FontVertexArray->AddVertexBuffer(s_Data.FontVertexBuffer);

		// Indices
		s_Data.FontVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);

		// Vertices
		s_Data.FontVertices = new QuadVertex[s_Data.MaxVertices];

		s_Data.Fonts = new Ref<Font>[s_Data.MaxSamplers];

		const char* fontVertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			layout(location = 2) in vec4 a_Color;
			layout(location = 3) in int  a_TexIndex;

			uniform mat4 u_View;
			uniform mat4 u_Projection;

			out vec3 v_Position;
			out vec2 v_TexCoord;
			out vec4 v_Color;
			flat out int v_TexIndex;

			void main()
			{
				vec4 position = vec4(a_Position, 1.0);

				gl_Position = u_Projection * u_View * position;

				v_Position	= position.xyz;
				v_TexCoord	= a_TexCoord;
				v_Color		= a_Color;
				v_TexIndex	= a_TexIndex;
			}
		)";

		const char* fontFragmentSource = R"(
			#version 330 core

			out vec4 o_FragColor;

			uniform sampler2D u_Samplers[32];

			in vec3 v_Position;
			in vec2 v_TexCoord;
			in vec4 v_Color;
			flat in int v_TexIndex;

			void main()
			{
				// Glyph information is encoded in the red channel
				float texture = texture(u_Samplers[v_TexIndex], v_TexCoord.xy, 0).r;
				o_FragColor = v_Color * texture;
			}
		)";

		s_Data.FontShader = Shader::Create("defaultFont", fontVertexSource, fontFragmentSource);
		s_Data.FontShader->Attach();
		s_Data.FontShader->SetIntArray("u_Samplers", s_SetupData.Samplers, s_Data.MaxSamplers);
	}

	void Renderer::SetupLineRenderer()
	{
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
		});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

		// Vertices
		s_Data.LineVertices = new LineVertex[s_Data.MaxVertices];

		const char* lineVertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_View;
			uniform mat4 u_Projection;

			out vec4 v_Color;

			void main()
			{
				vec4 position = vec4(a_Position, 1.0);

				gl_Position = u_Projection * u_View * position;

				v_Color = a_Color;
			}
		)";

		const char* lineFragmentSource = R"(
			#version 330 core

			in vec4 v_Color;
			out vec4 o_FragColor;

			void main()
			{
				o_FragColor = v_Color;
			}
		)";

		s_Data.LineShader = Shader::Create("defaultSprite", lineVertexSource, lineFragmentSource);
	}

	void Renderer::Dispose()
	{
		delete[] s_Data.QuadVertices;
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
		s_Stats.CharactersDrawn = 0;
		s_Stats.LinesDrawn = 0;
	}

	void Renderer::EndScene()
	{
		PushBatch();
	}

	void Renderer::StartBatch()
	{
		s_Data.QuadVertexPtr = s_Data.QuadVertices;
		s_Data.QuadIndexCount = 0;
		s_Data.QuadSamplerIndex = 1; // 0 is reserved for default white texture
		
		s_Data.FontVertexPtr = s_Data.FontVertices;
		s_Data.FontIndexCount = 0;
		s_Data.FontSamplerIndex = 0;

		s_Data.LineVertexPtr = s_Data.LineVertices;
	}

	void Renderer::PushBatch()
	{
		if (s_Data.QuadIndexCount > 0)
		{
			size_t size = ((uint8_t*)s_Data.QuadVertexPtr - (uint8_t*)s_Data.QuadVertices);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertices, size);

			for (int32_t i = 0; i < s_Data.QuadSamplerIndex; i++)
				s_Data.Textures[i]->Attach(i);

			s_Data.QuadShader->Attach();
			s_Data.QuadShader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
			s_Data.QuadShader->SetMat4("u_View", s_SceneData.ViewMatrix);

			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

			s_Stats.DrawCalls++;
		}

		if (s_Data.FontIndexCount > 0)
		{
			size_t size = ((uint8_t*)s_Data.FontVertexPtr - (uint8_t*)s_Data.FontVertices);
			s_Data.FontVertexBuffer->SetData(s_Data.FontVertices, size);

			for (int32_t i = 0; i < s_Data.FontSamplerIndex; i++)
				s_Data.Fonts[i]->Attach(i);

			s_Data.FontShader->Attach();
			s_Data.FontShader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
			s_Data.FontShader->SetMat4("u_View", s_SceneData.ViewMatrix);

			RenderCommand::DrawIndexed(s_Data.FontVertexArray, s_Data.FontIndexCount);

			s_Stats.DrawCalls++;
		}

		{
			size_t count = s_Data.LineVertexPtr - s_Data.LineVertices;
			if (count > 0)
			{
				s_Data.LineVertexBuffer->SetData(s_Data.LineVertices, count * sizeof(LineVertex));

				s_Data.LineShader->Attach();
				s_Data.LineShader->SetMat4("u_Projection", s_SceneData.ProjectionMatrix);
				s_Data.LineShader->SetMat4("u_View", s_SceneData.ViewMatrix);

				RenderCommand::DrawLines(s_Data.LineVertexArray, count);

				s_Stats.DrawCalls++;
			}
		}
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

		s_Data.DefaultTexture->Attach();

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

		va->Attach();
		RenderCommand::DrawIndexed(va);
	}

	void Renderer::DrawSprite(const Ref<Texture2D>& texture, const glm::mat4& transform)
	{
		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		DrawSprite(texture, textureCoords, transform);
	}

	void Renderer::DrawSprite(const SubTexture2D& subTexture, const glm::mat4& transform)
	{
		DrawSprite(subTexture.GetTexture(), subTexture.GetCoords(), transform);
	}

	void Renderer::DrawSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data.QuadSamplerIndex; i++)
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
			if (s_Data.QuadSamplerIndex >= s_Data.MaxSamplers)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data.QuadSamplerIndex;
			s_Data.Textures[s_Data.QuadSamplerIndex] = texture;
			s_Data.QuadSamplerIndex++;
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexPtr->Position	= transform * s_SpriteVertexPositions[i];
			s_Data.QuadVertexPtr->TexCoord	= texCoords[i];
			s_Data.QuadVertexPtr->Color		= { 1.0f, 1.0f, 1.0f, 1.0f };
			s_Data.QuadVertexPtr->TexIndex	= textureIndex;

			s_Data.QuadVertexPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer::DrawSprite(const glm::vec4& color, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexPtr->Position	= transform * s_SpriteVertexPositions[i];
			s_Data.QuadVertexPtr->TexCoord	= { 0.0f, 0.0f };
			s_Data.QuadVertexPtr->Color		= color;
			s_Data.QuadVertexPtr->TexIndex	= 0;

			s_Data.QuadVertexPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer::DrawText(const Text& text, const glm::mat4& transform)
	{
		DrawText(text.GetFont(), text.GetString(), text.GetColor(), { 1.0f, 1.0f }, text.GetOrigin(), transform);
	}

	void Renderer::DrawText(const Ref<Font>& font, const std::string& text, const glm::vec4& color, const glm::vec2& size, const glm::vec2& origin, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data.FontIndexCount >= s_Data.MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data.FontSamplerIndex; i++)
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
			// If not enough space to attach new font texture
			if (s_Data.FontSamplerIndex >= s_Data.MaxSamplers)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data.FontSamplerIndex;
			s_Data.Fonts[s_Data.FontSamplerIndex] = font;
			s_Data.FontSamplerIndex++;
		}

		constexpr float scale = 0.001f;

		float x = -origin.x;
		float y = -origin.y;
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

			s_Data.FontVertexPtr->Position	= transform * glm::vec4(xpos + w, -ypos, 0.0f, 1.0f);
			s_Data.FontVertexPtr->TexCoord	= { tx + ch.Size.x / atlasWidth, 0.0f };
			s_Data.FontVertexPtr->Color		= color;
			s_Data.FontVertexPtr->TexIndex	= textureIndex;
			s_Data.FontVertexPtr++;

			s_Data.FontVertexPtr->Position	= transform * glm::vec4(xpos, -ypos, 0.0f, 1.0f);
			s_Data.FontVertexPtr->TexCoord	= { tx, 0.0f };
			s_Data.FontVertexPtr->Color		= color;
			s_Data.FontVertexPtr->TexIndex	= textureIndex;
			s_Data.FontVertexPtr++;

			s_Data.FontVertexPtr->Position	= transform * glm::vec4(xpos, -ypos - h, 0.0f, 1.0f);
			s_Data.FontVertexPtr->TexCoord	= { tx, ch.Size.y / atlasHeight };
			s_Data.FontVertexPtr->Color		= color;
			s_Data.FontVertexPtr->TexIndex	= textureIndex;
			s_Data.FontVertexPtr++;

			s_Data.FontVertexPtr->Position	= transform * glm::vec4(xpos + w, -ypos - h, 0.0f, 1.0f);
			s_Data.FontVertexPtr->TexCoord	= { tx + ch.Size.x / atlasWidth, ch.Size.y / atlasHeight };
			s_Data.FontVertexPtr->Color		= color;
			s_Data.FontVertexPtr->TexIndex	= textureIndex;
			s_Data.FontVertexPtr++;

			s_Data.FontIndexCount += 6;
			s_Stats.CharactersDrawn++;
		}
	}

	void Renderer::DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& beginColor, const glm::vec4& endColor)
	{
		// Check if buffer is full
		size_t count = s_Data.LineVertexPtr - s_Data.LineVertices;
		if (count >= s_Data.MaxVertices)
		{
			PushBatch();
			StartBatch();
		}

		s_Data.LineVertexPtr->Position = p1;
		s_Data.LineVertexPtr->Color    = beginColor;
		s_Data.LineVertexPtr++;

		s_Data.LineVertexPtr->Position = p2;
		s_Data.LineVertexPtr->Color    = endColor;
		s_Data.LineVertexPtr++;

		s_Stats.LinesDrawn++;
	}

	void Renderer::OnWindowResized(uint32_t width, uint32_t height)
	{
		s_ViewportWidth = width; s_ViewportHeight = height;
		RenderCommand::SetViewport(0, 0, width, height);
	}
}

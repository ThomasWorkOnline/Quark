#include "Renderer2D.h"
#include "RenderCommand.h"

#include <sstream>

namespace Quark {

	/*      Index order
	 *  1    2      1 <- 2
	 *      /^      |   ^
	 *     / |      |  /
	 *    /  |      | /
	 *   v	 |      v/
	 *  0 -> 3      0    3
	 */

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

	struct Renderer2DData
	{
		static constexpr uint32_t MaxQuads    = 20000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices  = MaxQuads * 6;

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

	Renderer2DStats Renderer2D::s_Stats;

	static Renderer2DData* s_Data;
	struct Renderer2D::Renderer2DSetupData
	{
		uint32_t* Indices = nullptr;
		int32_t* Samplers = nullptr;

		Renderer2DSetupData()
		{
			// Samplers
			Samplers = new int32_t[s_Data->MaxSamplers];
			for (uint32_t i = 0; i < s_Data->MaxSamplers; i++)
				Samplers[i] = i;

			// Indices
			Indices = new uint32_t[Renderer2DData::MaxIndices];

			uint32_t offset = 0;
			for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
			{
				Indices[i + 0] = offset + 0;
				Indices[i + 1] = offset + 3;
				Indices[i + 2] = offset + 2;
				Indices[i + 3] = offset + 0;
				Indices[i + 4] = offset + 2;
				Indices[i + 5] = offset + 1;

				offset += 4;
			}
		}

		~Renderer2DSetupData()
		{
			delete[] Indices;
			delete[] Samplers;
		}
	};
	
	void Renderer2D::DrawSprite(const Ref<Texture2D>& texture, const glm::mat4& transform)
	{
		static constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		DrawSprite(texture, textureCoords, transform);
	}

	void Renderer2D::DrawSprite(const SubTexture2D& subTexture, const glm::mat4& transform)
	{
		DrawSprite(subTexture.GetTexture(), subTexture.GetCoords(), transform);
	}

	void Renderer2D::DrawSprite(const Ref<Texture2D>& texture, const glm::vec2* texCoords, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->QuadSamplerIndex; i++)
		{
			if (*s_Data->Textures[i] == *texture)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0)
		{
			// If not enough space to attach new texture
			if (s_Data->QuadSamplerIndex >= s_Data->MaxSamplers)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data->QuadSamplerIndex;
			s_Data->Textures[textureIndex] = texture;
			s_Data->QuadSamplerIndex++;
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexPtr->Position = transform * s_SpriteVertexPositions[i];
			s_Data->QuadVertexPtr->TexCoord = texCoords[i];
			s_Data->QuadVertexPtr->Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			s_Data->QuadVertexPtr->TexIndex = textureIndex;

			s_Data->QuadVertexPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer2D::DrawSprite(const glm::vec4& color, const glm::mat4& transform)
	{
		// Check if buffer is full
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexPtr->Position = transform * s_SpriteVertexPositions[i];
			s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
			s_Data->QuadVertexPtr->Color = color;
			s_Data->QuadVertexPtr->TexIndex = 0;

			s_Data->QuadVertexPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& beginColor, const glm::vec4& endColor)
	{
		// Check if buffer is full
		size_t count = s_Data->LineVertexPtr - s_Data->LineVertices;
		if (count >= Renderer2DData::MaxVertices)
		{
			PushBatch();
			StartBatch();
		}

		s_Data->LineVertexPtr->Position = p1;
		s_Data->LineVertexPtr->Color = beginColor;
		s_Data->LineVertexPtr++;

		s_Data->LineVertexPtr->Position = p2;
		s_Data->LineVertexPtr->Color = endColor;
		s_Data->LineVertexPtr++;

		s_Stats.LinesDrawn++;
	}

	void Renderer2D::DrawText(std::string_view text, const TextRenderTraits& traits)
	{
		// Check if buffer is full
		if (s_Data->FontIndexCount >= Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->FontSamplerIndex; i++)
		{
			if (*s_Data->Fonts[i] == *traits.FontStyle)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0)
		{
			// If not enough space to attach new font texture
			if (s_Data->FontSamplerIndex >= s_Data->MaxSamplers)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data->FontSamplerIndex;
			s_Data->Fonts[textureIndex] = traits.FontStyle;
			s_Data->FontSamplerIndex++;
		}

		auto [x, y] = traits.GetOrigin();

		float atlasWidth = traits.FontStyle->GetAtlasWidth();
		float atlasHeight = traits.FontStyle->GetAtlasHeight();

		for (auto it = text.begin(); it != text.end(); it++)
		{
			auto& g = traits.FontStyle->GetGlyph(*it);

			int32_t xpos = (x + g.Bearing.x);
			int32_t ypos = (-y - g.Bearing.y);
			int32_t w = g.Size.x;
			int32_t h = g.Size.y;
			float tx = (float)g.OffsetX / atlasWidth;

			x += (g.Advance.x >> 6);
			y += (g.Advance.y >> 6);

			if (!w || !h)
				continue;

			s_Data->FontVertexPtr->Position = glm::vec4(xpos + w, -ypos, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx + g.Size.x / atlasWidth, 0.0f };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontVertexPtr->Position = glm::vec4(xpos, -ypos, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx, 0.0f };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontVertexPtr->Position = glm::vec4(xpos, -ypos - h, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx, g.Size.y / atlasHeight };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontVertexPtr->Position = glm::vec4(xpos + w, -ypos - h, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx + g.Size.x / atlasWidth, g.Size.y / atlasHeight };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontIndexCount += 6;
			s_Stats.QuadsDrawn++;
		}
	}

	void Renderer2D::DrawText(const Text& text)
	{
		DrawText(text.GetText(), text.GetRenderTraits());
	}

	void Renderer2D::DrawTextInput(const TextInput& input)
	{
		DrawText(input.GetText(), input.GetRenderTraits());
	}

	void Renderer2D::Initialize()
	{
		QK_SCOPE_TIMER(Renderer2D::Initialize);

		s_Data = new Renderer2DData();
		s_Data->MaxSamplers = RenderCommand::GetMaxTextureSlots();

		Renderer2DSetupData setupData;

		SetupQuadRenderer(setupData);
		SetupFontRenderer(setupData);
		SetupLineRenderer();
	}

	void Renderer2D::Dispose()
	{
		QK_SCOPE_TIMER(Renderer2D::Dispose);

		delete[] s_Data->QuadVertices;
		delete[] s_Data->FontVertices;
		delete[] s_Data->LineVertices;
		delete[] s_Data->Textures;
		delete[] s_Data->Fonts;
		delete s_Data;
	}

	void Renderer2D::SetupQuadRenderer(Renderer2DSetupData& setupData)
	{
		QK_SCOPE_TIMER(Renderer2D::SetupQuadRenderer);

		s_Data->QuadVertexArray = VertexArray::Create();

		s_Data->QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
		s_Data->QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_Tint"     },
			{ ShaderDataType::Int,    "a_TexIndex" }
		});
		s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

		// Indices
		s_Data->QuadIndexBuffer = IndexBuffer::Create(setupData.Indices, Renderer2DData::MaxIndices);
		s_Data->QuadVertexArray->SetIndexBuffer(s_Data->QuadIndexBuffer);

		// Vertices
		s_Data->QuadVertices = new QuadVertex[Renderer2DData::MaxVertices];

		// Textures
		s_Data->Textures = new Ref<Texture2D>[s_Data->MaxSamplers];

		uint32_t textureColor = 0xffffffff;
		TextureSpecification spec = { 1, 1, 1,
			TextureDataFormat::RGBA,
			TextureInternalFormat::RGBA8,
			TextureFilteringMode::Nearest, TextureFilteringMode::Nearest, TextureTilingMode::Repeat
		};

		s_Data->DefaultTexture = Texture2D::Create(spec);
		s_Data->DefaultTexture->SetData(&textureColor, sizeof(uint32_t));

		s_Data->Textures[0] = s_Data->DefaultTexture;

		const char* spriteVertexSource = R"(
			#version 420 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			layout(location = 2) in vec4 a_Tint;
			layout(location = 3) in int  a_TexIndex;

			layout(std140, binding = 0) uniform Camera
			{
				mat4 u_ViewProjection;
			};

			out VertexOutput
			{
				vec2 TexCoord;
				vec4 Tint;
				flat int TexIndex;
			} v_Output;

			void main()
			{
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				v_Output.TexCoord = a_TexCoord;
				v_Output.Tint     = a_Tint;
				v_Output.TexIndex = a_TexIndex;
			}
		)";

		std::stringstream spriteFragmentSource;
		spriteFragmentSource << R"(
			#version 330 core

			uniform sampler2D u_Samplers[
		)";

		spriteFragmentSource << s_Data->MaxSamplers;
		spriteFragmentSource << R"(];

			in VertexOutput
			{
				vec2 TexCoord;
				vec4 Tint;
				flat int TexIndex;
			} v_Input;

			out vec4 o_Color;

			void main()
			{
				int lvalueIndex = v_Input.TexIndex;
				vec4 color = texture(u_Samplers[lvalueIndex], v_Input.TexCoord.xy);
				o_Color = color * v_Input.Tint;
			}
		)";

		s_Data->QuadShader = Shader::Create("defaultSprite", spriteVertexSource, spriteFragmentSource.str());
		s_Data->QuadShader->Attach();
		s_Data->QuadShader->SetIntArray("u_Samplers", setupData.Samplers, s_Data->MaxSamplers);
	}

	void Renderer2D::SetupFontRenderer(Renderer2DSetupData& setupData)
	{
		QK_SCOPE_TIMER(Renderer2D::SetupFontRenderer);

		s_Data->FontVertexArray = VertexArray::Create();

		s_Data->FontVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
		s_Data->FontVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_TexIndex" }
		});
		s_Data->FontVertexArray->AddVertexBuffer(s_Data->FontVertexBuffer);

		// Indices
		s_Data->FontVertexArray->SetIndexBuffer(s_Data->QuadIndexBuffer);

		// Vertices
		s_Data->FontVertices = new QuadVertex[Renderer2DData::MaxVertices];
		s_Data->Fonts = new Ref<Font>[s_Data->MaxSamplers];

		const char* fontVertexSource = R"(
			#version 420 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			layout(location = 2) in vec4 a_Color;
			layout(location = 3) in int  a_TexIndex;

			layout(std140, binding = 0) uniform Camera
			{
				mat4 u_ViewProjection;
			};

			out VertexOutput
			{
				vec2 TexCoord;
				vec4 Color;
				flat int TexIndex;
			} v_Output;

			void main()
			{
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				v_Output.TexCoord = a_TexCoord;
				v_Output.Color    = a_Color;
				v_Output.TexIndex = a_TexIndex;
			}
		)";

		std::stringstream fontFragmentSource;
		fontFragmentSource << R"(
			#version 330 core

			uniform sampler2D u_Samplers[
		)";
			
		fontFragmentSource << s_Data->MaxSamplers;
		fontFragmentSource << R"(];

			in VertexOutput
			{
				vec2 TexCoord;
				vec4 Color;
				flat int TexIndex;
			} v_Input;

			out vec4 o_Color;

			void main()
			{
				// Glyph information is encoded in the red channel
				int lvalueIndex = v_Input.TexIndex;
				float texel = texture(u_Samplers[lvalueIndex], v_Input.TexCoord.xy, 0).r;
				
				if (texel == 0)
					discard;

				o_Color = v_Input.Color * texel;
			}
		)";

		s_Data->FontShader = Shader::Create("defaultFont", fontVertexSource, fontFragmentSource.str());
		s_Data->FontShader->Attach();
		s_Data->FontShader->SetIntArray("u_Samplers", setupData.Samplers, s_Data->MaxSamplers);
	}

	void Renderer2D::SetupLineRenderer()
	{
		QK_SCOPE_TIMER(Renderer2D::SetupLineRenderer);

		s_Data->LineVertexArray = VertexArray::Create();

		s_Data->LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(LineVertex));
		s_Data->LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
		});
		s_Data->LineVertexArray->AddVertexBuffer(s_Data->LineVertexBuffer);

		// Vertices
		s_Data->LineVertices = new LineVertex[Renderer2DData::MaxVertices];

		const char* lineVertexSource = R"(
			#version 420 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			layout(std140, binding = 0) uniform Camera
			{
				mat4 u_ViewProjection;
			};

			out vec4 v_Color;

			void main()
			{
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				v_Color = a_Color;
			}
		)";

		const char* lineFragmentSource = R"(
			#version 330 core

			in vec4 v_Color;
			out vec4 o_Color;

			void main()
			{
				o_Color = v_Color;
			}
		)";

		s_Data->LineShader = Shader::Create("defaultLine", lineVertexSource, lineFragmentSource);
	}

	void Renderer2D::BeginScene()
	{
		StartBatch();
		ResetStats();
	}

	void Renderer2D::EndScene()
	{
		PushBatch();
	}

	void Renderer2D::StartBatch()
	{
		s_Data->QuadVertexPtr    = s_Data->QuadVertices;
		s_Data->QuadIndexCount   = 0;
		s_Data->QuadSamplerIndex = 1; // 0 is reserved for default texture
		
		s_Data->FontVertexPtr    = s_Data->FontVertices;
		s_Data->FontIndexCount   = 0;
		s_Data->FontSamplerIndex = 0;

		s_Data->LineVertexPtr    = s_Data->LineVertices;
	}

	void Renderer2D::PushBatch()
	{
		if (s_Data->QuadIndexCount > 0)
		{
			size_t size = ((uint8_t*)s_Data->QuadVertexPtr - (uint8_t*)s_Data->QuadVertices);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertices, size);

			for (uint32_t i = 0; i < s_Data->QuadSamplerIndex; i++)
				s_Data->Textures[i]->Attach(i);

			s_Data->QuadShader->Attach();
			RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);

			s_Stats.DrawCalls++;
		}

		if (s_Data->FontIndexCount > 0)
		{
			size_t size = ((uint8_t*)s_Data->FontVertexPtr - (uint8_t*)s_Data->FontVertices);
			s_Data->FontVertexBuffer->SetData(s_Data->FontVertices, size);

			for (uint32_t i = 0; i < s_Data->FontSamplerIndex; i++)
				s_Data->Fonts[i]->Attach(i);

			s_Data->FontShader->Attach();
			RenderCommand::DrawIndexed(s_Data->FontVertexArray, s_Data->FontIndexCount);

			s_Stats.DrawCalls++;
		}

		{
			size_t count = s_Data->LineVertexPtr - s_Data->LineVertices;
			if (count > 0)
			{
				s_Data->LineVertexBuffer->SetData(s_Data->LineVertices, count * sizeof(LineVertex));

				s_Data->LineShader->Attach();
				RenderCommand::DrawLines(s_Data->LineVertexArray, count);

				s_Stats.DrawCalls++;
			}
		}
	}

	void Renderer2D::ResetStats()
	{
		s_Stats.DrawCalls = 0;
		s_Stats.QuadsDrawn = 0;
		s_Stats.LinesDrawn = 0;
	}
}

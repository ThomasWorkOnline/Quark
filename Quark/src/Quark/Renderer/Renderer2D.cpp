#include "qkpch.h"
#include "Renderer2D.h"

#include "Quark/Core/Application.h"
#include "Quark/Filesystem/Filesystem.h"

#include "Sampler.h"
#include "GraphicsAPI.h"

namespace Quark {

	/*
		- Coordinate System -

		(-,+)     |      (+,+)
		          |
		          |
		----------+----------
		          |
		          |
		(-,-)     |      (+,-)

		---- Index order ----

		3    2         3 <- 2
		    /^         |   ^
		   / |         |  /
		  /  |         | /
		 v   |         v/
		0 -> 1         0    1
	*/

	static constexpr Vec4f s_QuadVertexPositions[4] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	static constexpr uint32_t s_QuadIndices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	static constexpr Vec2f s_TextureCoords[4] = {
		{ 0.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 0.0f },
		{ 0.0f, 0.0f }
	};

	static const BufferLayout s_QuadVertexLayout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Float4, "a_Color"    },
		{ ShaderDataType::Int,    "a_TexIndex" }
	};

	struct QuadVertex
	{
		Vec3f Position;
		Vec2f TexCoord;
		Vec4f Color;
		int32_t TexIndex;
	};

	static const BufferLayout s_LineVertexLayout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color"    },
	};

	struct LineVertex
	{
		Vec3f Position;
		Vec4f Color;
	};

	struct Renderer2DData
	{
		static constexpr uint32_t MaxQuads    = 20000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;
		static constexpr uint32_t MaxIndices  = MaxQuads * 6;

		static constexpr uint32_t TabSpaces = 4;

		QuadVertex* QuadVertexPtr = nullptr;
		QuadVertex* QuadVertices  = nullptr;
		uint32_t QuadIndexCount   = 0;

		Scope<Shader> QuadShader;
		Scope<IndexBuffer> QuadIndexBuffer;
		Scope<VertexBuffer> QuadVertexBuffer;
		Scope<Pipeline> QuadRendererPipeline;

		LineVertex* LineVertexPtr = nullptr;
		LineVertex* LineVertices  = nullptr;

		Scope<Shader> LineShader;
		Scope<VertexBuffer> LineVertexBuffer;
		Scope<Pipeline> LineRendererPipeline;

		uint32_t MaxSamplerDestinations = 0;
		uint32_t TextureSamplerIndex = 1; // Next texture slot to be attached, 0 is reserved for default texture
		std::vector<Scope<Sampler>> SamplerPool;

		Scope<Texture2D> DefaultTexture;
		const Texture** Textures = nullptr;
		const Sampler** Samplers = nullptr;

		// Ensure std140 layout
		struct CameraData
		{
			Mat4f ViewProjection;
		};

		struct TextRendererData
		{
			int StartX = 0;
			int StartY = 0;
			int X = StartX, Y = StartY;

			const Font* Font = nullptr;
		};

		TextRendererData TextData;
		CameraData CameraBufferData{};
		Scope<UniformBuffer> CameraUniformBuffer;
	};

	Renderer2DStats Renderer2D::s_Stats;
	static Renderer2DData* s_Data = nullptr;

	static void SetupQuadRenderer();
	static void SetupLineRenderer();

	void Renderer2D::BeginScene(const Camera& camera, const Transform3DComponent& cameraTransform)
	{
		Mat4f rotate = glm::toMat4(cameraTransform.Orientation);
		Mat4f view = glm::translate(rotate, (Vec3f)-cameraTransform.Position);

		BeginScene(camera.GetProjection(), view);
	}

	void Renderer2D::BeginScene(const Mat4f& cameraProjection, const Mat4f& cameraView)
	{
		QK_ASSERT_RENDER_THREAD();

		s_Data->CameraBufferData.ViewProjection = cameraProjection * cameraView;
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBufferData, sizeof(s_Data->CameraBufferData));

		ResetStats();
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		PushBatch();
	}
	
	void Renderer2D::DrawSprite(const Texture* texture, const Vec4f& tint, const Mat4f& transform)
	{
		DrawSprite(texture, s_TextureCoords, tint, transform);
	}

	void Renderer2D::DrawSprite(const Texture* texture, float aspectRatio, const Vec4f& tint)
	{
		Mat4f transform = glm::scale(Mat4f(1.0f), Vec3f(aspectRatio, 1.0f, 1.0f));
		DrawSprite(texture, tint, transform);
	}

	void Renderer2D::DrawSprite(const Texture* texture, const Vec2f* texCoords, const Vec4f& tint, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->TextureSamplerIndex; i++)
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
			if (s_Data->TextureSamplerIndex >= s_Data->MaxSamplerDestinations)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data->TextureSamplerIndex;
			s_Data->Textures[textureIndex] = texture;
			s_Data->Samplers[textureIndex] = s_Data->SamplerPool[textureIndex].get();
			s_Data->TextureSamplerIndex++;
		}

		// Check if buffer is full
		if (s_Data->QuadIndexCount + 6 > Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexPtr->Position = transform * s_QuadVertexPositions[i];
			s_Data->QuadVertexPtr->TexCoord = texCoords[i];
			s_Data->QuadVertexPtr->Color    = tint;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;

			s_Data->QuadVertexPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer2D::DrawSprite(const SubTexture2D& subTexture, const Vec4f& tint, const Mat4f& transform)
	{
		DrawSprite(subTexture.GetTexture(), subTexture.GetCoords(), tint, transform);
	}

	void Renderer2D::DrawQuad(const Vec4f& color, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if buffer is full
		if (s_Data->QuadIndexCount + 6 > Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexPtr->Position = transform * s_QuadVertexPositions[i];
			s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = 0;

			s_Data->QuadVertexPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer2D::DrawQuad(const Vec3f& start, const Vec3f& end, const Vec4f& color)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if buffer is full
		if (s_Data->QuadIndexCount + 6 > Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		// Vertex [-, -]
		s_Data->QuadVertexPtr->Position = start;
		s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
		s_Data->QuadVertexPtr->Color    = color;
		s_Data->QuadVertexPtr->TexIndex = 0;
		s_Data->QuadVertexPtr++;

		// Vertex [+, -]
		s_Data->QuadVertexPtr->Position = { end.x, start.y, start.z };
		s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
		s_Data->QuadVertexPtr->Color    = color;
		s_Data->QuadVertexPtr->TexIndex = 0;
		s_Data->QuadVertexPtr++;

		// Vertex [+, +]
		s_Data->QuadVertexPtr->Position = end;
		s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
		s_Data->QuadVertexPtr->Color    = color;
		s_Data->QuadVertexPtr->TexIndex = 0;
		s_Data->QuadVertexPtr++;

		// Vertex [-, +]
		s_Data->QuadVertexPtr->Position = { start.x, end.y, start.z };
		s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
		s_Data->QuadVertexPtr->Color    = color;
		s_Data->QuadVertexPtr->TexIndex = 0;
		s_Data->QuadVertexPtr++;

		s_Data->QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer2D::DrawLine(const Vec3f& p1, const Vec3f& p2, const Vec4f& beginColor, const Vec4f& endColor)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if buffer is full
		uint32_t count = (uint32_t)(s_Data->LineVertexPtr - s_Data->LineVertices);
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

	void Renderer2D::BeginText()
	{
		QK_ASSERT_RENDER_THREAD();
		s_Data->TextData = {};
	}

	void Renderer2D::EndText()
	{
	}

	void Renderer2D::PushText(std::string_view text, const Font* font, const Vec4f& color, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->TextureSamplerIndex; i++)
		{
			if (*s_Data->Textures[i] == *font)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0)
		{
			// If not enough space to attach new font texture
			if (s_Data->TextureSamplerIndex >= s_Data->MaxSamplerDestinations)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data->TextureSamplerIndex;
			s_Data->Textures[textureIndex] = font;
			s_Data->Samplers[textureIndex] = s_Data->SamplerPool[textureIndex].get();
			s_Data->TextureSamplerIndex++;
		}

		uint32_t atlasWidth = font->GetAtlasWidth();
		uint32_t atlasHeight = font->GetAtlasHeight();

		for (auto charcode : text)
		{
			if (charcode < 32)
			{
				switch (charcode)
				{
					// Newline and carriage return
					case '\n':
						s_Data->TextData.X = s_Data->TextData.StartX;
						s_Data->TextData.Y -= atlasHeight;
						break;

					// Tabs
					case '\t':
						s_Data->TextData.X += 32 * Renderer2DData::TabSpaces;
						break;
				}

				continue;
			}

			auto& g = font->GetGlyph(charcode);

			int xpos = (s_Data->TextData.X + g.Bearing.x);
			int ypos = (s_Data->TextData.Y + g.Bearing.y);

			s_Data->TextData.X += (g.Advance.x >> 6);

			if (g.Size.x == 0 || g.Size.y == 0)
				continue;

			float tx = (float)g.OffsetX / (float)atlasWidth;
			float w = (float)g.Size.x;
			float h = (float)g.Size.y;

			// Check if buffer is full
			if (s_Data->QuadIndexCount + 6 > Renderer2DData::MaxIndices)
			{
				PushBatch();
				StartBatch();
			}

			// Vertex [-, -]
			s_Data->QuadVertexPtr->Position = transform * Vec4f(xpos, (ypos - h), 0.0f, 1.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx, h / (float)atlasHeight };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			// Vertex [+, -]
			s_Data->QuadVertexPtr->Position = transform * Vec4f(xpos + w, (ypos - h), 0.0f, 1.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx + w / (float)atlasWidth, h / (float)atlasHeight };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			// Vertex [+, +]
			s_Data->QuadVertexPtr->Position = transform * Vec4f(xpos + w, ypos, 0.0f, 1.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx + w / (float)atlasWidth, 0.0f };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			// Vertex [-, +]
			s_Data->QuadVertexPtr->Position = transform * Vec4f(xpos, ypos, 0.0f, 1.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx, 0.0f };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			s_Data->QuadIndexCount += 6;
			s_Stats.QuadsDrawn++;
		}
	}

	void Renderer2D::PushText(std::string_view text, const Font* font, const Vec4f& color, const Vec3f& position)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if texture exists in samplers
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < s_Data->TextureSamplerIndex; i++)
		{
			if (*s_Data->Textures[i] == *font)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture was not found in samplers
		if (textureIndex == 0)
		{
			// If not enough space to attach new font texture
			if (s_Data->TextureSamplerIndex >= s_Data->MaxSamplerDestinations)
			{
				PushBatch();
				StartBatch();
			}

			textureIndex = s_Data->TextureSamplerIndex;
			s_Data->Textures[textureIndex] = font;
			s_Data->Samplers[textureIndex] = s_Data->SamplerPool[textureIndex].get();
			s_Data->TextureSamplerIndex++;
		}

		uint32_t atlasWidth = font->GetAtlasWidth();
		uint32_t atlasHeight = font->GetAtlasHeight();

		for (auto charcode : text)
		{
			if (charcode < 32)
			{
				switch (charcode)
				{
					// Newline and carriage return
					case '\n':
						s_Data->TextData.X = s_Data->TextData.StartX;
						s_Data->TextData.Y -= atlasHeight;
						break;

					// Tabs
					case '\t':
						s_Data->TextData.X += 32 * Renderer2DData::TabSpaces;
						break;
				}

				continue;
			}

			auto& g = font->GetGlyph(charcode);

			int xpos = (s_Data->TextData.X + g.Bearing.x);
			int ypos = (s_Data->TextData.Y + g.Bearing.y);

			s_Data->TextData.X += (g.Advance.x >> 6);

			if (g.Size.x == 0 || g.Size.y == 0)
				continue;

			float tx = (float)g.OffsetX / (float)atlasWidth;
			float w = (float)g.Size.x;
			float h = (float)g.Size.y;

			// Check if buffer is full
			if (s_Data->QuadIndexCount + 6 > Renderer2DData::MaxIndices)
			{
				PushBatch();
				StartBatch();
			}

			// Vertex [-, -]
			s_Data->QuadVertexPtr->Position = position + Vec3f(xpos, (ypos - h), 0.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx, h / (float)atlasHeight };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			// Vertex [+, -]
			s_Data->QuadVertexPtr->Position = position + Vec3f(xpos + w, (ypos - h), 0.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx + w / (float)atlasWidth, h / (float)atlasHeight };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			// Vertex [+, +]
			s_Data->QuadVertexPtr->Position = position + Vec3f(xpos + w, ypos, 0.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx + w / (float)atlasWidth, 0.0f };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			// Vertex [-, +]
			s_Data->QuadVertexPtr->Position = position + Vec3f(xpos, ypos, 0.0f);
			s_Data->QuadVertexPtr->TexCoord = { tx, 0.0f };
			s_Data->QuadVertexPtr->Color    = color;
			s_Data->QuadVertexPtr->TexIndex = textureIndex;
			s_Data->QuadVertexPtr++;

			s_Data->QuadIndexCount += 6;
			s_Stats.QuadsDrawn++;
		}
	}

	void Renderer2D::DrawText(std::string_view text, const Font* font, const Vec4f& color, const Mat4f& transform)
	{
		BeginText();
			PushText(text, font, color, transform);
		EndText();
	}

	void Renderer2D::DrawText(std::string_view text, const Font* font, const Vec4f& color, const Vec3f& position)
	{
		BeginText();
			PushText(text, font, color, position);
		EndText();
	}

	void Renderer2D::DrawText(const Text& text, const Mat4f& transform)
	{
		auto& traits = text.GetStyle();
		BeginText();
			PushText(text, traits.Font.get(), traits.Color, transform);
		EndText();
	}

	void Renderer2D::StartBatch()
	{
		s_Data->QuadVertexPtr       = s_Data->QuadVertices;
		s_Data->QuadIndexCount      = 0;
		s_Data->TextureSamplerIndex = 1; // 0 is reserved for default texture
		
		s_Data->LineVertexPtr       = s_Data->LineVertices;
	}

	void Renderer2D::PushBatch()
	{
		if (s_Data->QuadIndexCount > 0)
		{
			Renderer::BindPipeline(s_Data->QuadRendererPipeline.get());

			size_t size = ((uint8_t*)s_Data->QuadVertexPtr - (uint8_t*)s_Data->QuadVertices);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertices, size);

			while (s_Data->TextureSamplerIndex != s_Data->MaxSamplerDestinations)
			{
				s_Data->Textures[s_Data->TextureSamplerIndex] = s_Data->DefaultTexture.get();
				s_Data->Samplers[s_Data->TextureSamplerIndex] = s_Data->SamplerPool[s_Data->TextureSamplerIndex].get();
				s_Data->TextureSamplerIndex++;
			}

			for (uint32_t i = 0; i < s_Data->MaxSamplerDestinations; i++)
				Renderer::BindTexture(s_Data->Textures[i], s_Data->Samplers[i], 1, i);

			Renderer::BindUniformBuffer(s_Data->CameraUniformBuffer.get(), 0);
			Renderer::BindDescriptorSets();

			Renderer::DrawIndexed(s_Data->QuadVertexBuffer.get(), s_Data->QuadIndexBuffer.get(), s_Data->QuadIndexCount);
			s_Stats.DrawCalls++;
		}

		if (uint32_t vertexCount = (uint32_t)(s_Data->LineVertexPtr - s_Data->LineVertices))
		{
			Renderer::BindPipeline(s_Data->LineRendererPipeline.get());

			size_t size = ((uint8_t*)s_Data->LineVertexPtr - (uint8_t*)s_Data->LineVertices);
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertices, size);

			Renderer::SetLineWidth(1.0f);

			Renderer::BindUniformBuffer(s_Data->CameraUniformBuffer.get(), 0);
			Renderer::BindDescriptorSets();

			Renderer::Draw(s_Data->LineVertexBuffer.get(), vertexCount);
			s_Stats.DrawCalls++;
		}
	}

	void Renderer2D::ResetStats()
	{
		s_Stats.DrawCalls = 0;
		s_Stats.QuadsDrawn = 0;
		s_Stats.LinesDrawn = 0;
	}

	void Renderer2D::Initialize()
	{
		QK_PROFILE_FUNCTION();

		if (s_Data) return;

		s_Data = new Renderer2DData();

		// Samplers
		s_Data->MaxSamplerDestinations = Renderer::GetCapabilities().Sampler.MaxTextureUnits;

		// Camera buffer
		static constexpr size_t ubSize = sizeof(s_Data->CameraBufferData);
		s_Data->CameraUniformBuffer = UniformBuffer::Create(ubSize);

		SetupQuadRenderer();
		SetupLineRenderer();
	}

	void Renderer2D::Dispose()
	{
		QK_PROFILE_FUNCTION();

		if (!s_Data) return;

		delete[] s_Data->QuadVertices;
		delete[] s_Data->LineVertices;
		delete[] s_Data->Textures;
		delete[] s_Data->Samplers;

		delete s_Data;
		s_Data = nullptr;
	}

	static void SetupQuadRenderer()
	{
		QK_PROFILE_FUNCTION();

		s_Data->QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
		s_Data->QuadVertexBuffer->SetLayout(s_QuadVertexLayout);

		uint32_t* indices = new uint32_t[Renderer2DData::MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
		{
			indices[i + 0] = offset + s_QuadIndices[0];
			indices[i + 1] = offset + s_QuadIndices[1];
			indices[i + 2] = offset + s_QuadIndices[2];
			indices[i + 3] = offset + s_QuadIndices[3];
			indices[i + 4] = offset + s_QuadIndices[4];
			indices[i + 5] = offset + s_QuadIndices[5];

			offset += 4;
		}

		static constexpr IndexType indexType = GetIndexTypeBasedOnIndexCount(Renderer2DData::MaxIndices);
		s_Data->QuadIndexBuffer = IndexBuffer::Create(indices, Renderer2DData::MaxIndices, indexType);
		delete[] indices;

		s_Data->QuadVertices = new QuadVertex[Renderer2DData::MaxVertices];

		s_Data->Textures = new const Texture* [s_Data->MaxSamplerDestinations];
		std::memset(s_Data->Textures, 0, s_Data->MaxSamplerDestinations * sizeof(Texture*));

		s_Data->Samplers = new const Sampler* [s_Data->MaxSamplerDestinations];
		std::memset(s_Data->Textures, 0, s_Data->MaxSamplerDestinations * sizeof(Sampler*));

		{
			uint32_t textureColor = 0xffffffff;
			Texture2DSpecification spec = { 1, 1, 1, SampleCount::SampleCount1, ColorFormat::RGBA8 };

			s_Data->DefaultTexture = Texture2D::Create(spec);
			s_Data->DefaultTexture->SetData(&textureColor, sizeof(textureColor));
		}

		s_Data->SamplerPool.resize(s_Data->MaxSamplerDestinations);
		for (uint32_t i = 0; i < s_Data->MaxSamplerDestinations; i++)
		{
			SamplerSpecification spec;
			spec.RenderModes.MinFilteringMode = SamplerFilterMode::Linear;
			spec.RenderModes.MagFilteringMode = SamplerFilterMode::Nearest;
			spec.RenderModes.MipmapMode = SamplerMipmapMode::Linear;
			spec.RenderModes.AddressMode = SamplerAddressMode::ClampToEdge;

			s_Data->SamplerPool[i] = Sampler::Create(spec);
		}

		// Reserved sampler locations
		s_Data->Textures[0] = s_Data->DefaultTexture.get();
		s_Data->Samplers[0] = s_Data->SamplerPool[0].get();

		{
			auto& coreDirectory = Application::Get()->GetSpecification().CoreDir;
			auto spriteVertexSource = Filesystem::ReadTextFile((coreDirectory / "assets/shaders/version/4.50/Sprite.vert").string());
			auto spriteFragmentSource = Filesystem::ReadTextFile((coreDirectory / "assets/shaders/version/4.50/Sprite.frag").string());

			s_Data->QuadShader = Shader::Create("defaultSprite", spriteVertexSource, spriteFragmentSource);
		}

		{
			PipelineSpecification spec;
			spec.Layout             = s_QuadVertexLayout;
			spec.Topology           = PrimitiveTopology::TriangleList;
			spec.DescriptorSetCount = Renderer::GetFramesInFlight();
			spec.RenderPass         = Renderer::GetRenderPass();
			spec.Shader             = s_Data->QuadShader.get();

			s_Data->QuadRendererPipeline = Pipeline::Create(spec);
		}
	}

	static void SetupLineRenderer()
	{
		QK_PROFILE_FUNCTION();

		s_Data->LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(LineVertex));
		s_Data->LineVertexBuffer->SetLayout(s_LineVertexLayout);

		s_Data->LineVertices = new LineVertex[Renderer2DData::MaxVertices];

		{
			auto& coreDirectory = Application::Get()->GetSpecification().CoreDir;
			auto lineVertexSource = Filesystem::ReadTextFile((coreDirectory / "assets/shaders/version/4.20/Line.vert").string());
			auto lineFragmentSource = Filesystem::ReadTextFile((coreDirectory / "assets/shaders/version/4.20/Line.frag").string());

			s_Data->LineShader = Shader::Create("defaultLine", lineVertexSource, lineFragmentSource);
		}

		{
			PipelineSpecification spec;
			spec.Layout             = s_LineVertexLayout;
			spec.Topology           = PrimitiveTopology::LineList;
			spec.DescriptorSetCount = Renderer::GetFramesInFlight();
			spec.RenderPass         = Renderer::GetRenderPass();
			spec.Shader             = s_Data->LineShader.get();

			s_Data->LineRendererPipeline = Pipeline::Create(spec);
		}
	}
}

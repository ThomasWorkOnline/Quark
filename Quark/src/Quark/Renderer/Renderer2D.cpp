#include "qkpch.h"
#include "Renderer2D.h"

#include "Quark/Core/Application.h"
#include "Quark/Filesystem/Filesystem.h"

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

	// NOTE: UVs are flipped vertically since the image loader
	// does not provide any way to flip it while loading
	// TODO: this is only applicable for OpenGL, change
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

		Scope<Texture2D> DefaultTexture;
		const Texture** Textures = nullptr;

		// Ensure std140 layout
		struct CameraData
		{
			Mat4f ViewProjection;
		};

		CameraData CameraBufferData{};
		Scope<UniformBuffer> CameraUniformBuffer;
		const UniformBuffer* UniformBuffers = nullptr;
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

	void Renderer2D::DrawSprite(const Vec4f& color, const Mat4f& transform)
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

	void Renderer2D::DrawText(std::string_view text, const Font* font, const Vec4f& color, const Mat4f& transform)
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
			s_Data->TextureSamplerIndex++;
		}

		int startX = 0;
		int startY = 0;
		int x = startX, y = startY;

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
						x = startX;
						y -= atlasHeight;
						break;

					// Tabs
					case '\t':
						x += 4 * 32;
						break;
				}

				continue;
			}

			auto& g = font->GetGlyph(charcode);

			int xpos = (x + g.Bearing.x);
			int ypos = (y + g.Bearing.y);

			x += (g.Advance.x >> 6);

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

	void Renderer2D::DrawText(const Text& text, const Mat4f& transform)
	{
		auto& traits = text.GetStyle();
		DrawText(text, traits.Font.get(), traits.Color, transform);
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
			size_t size = ((uint8_t*)s_Data->QuadVertexPtr - (uint8_t*)s_Data->QuadVertices);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertices, size);

			for (uint32_t i = 0; i < s_Data->TextureSamplerIndex; i++)
				s_Data->QuadRendererPipeline->SetTexture(s_Data->Textures[i], i);

			s_Data->QuadRendererPipeline->PushDescriptorSets();

			Renderer::BindPipeline(s_Data->QuadRendererPipeline.get());
			Renderer::Submit(s_Data->QuadVertexBuffer.get(), s_Data->QuadIndexBuffer.get(), s_Data->QuadIndexCount);
			s_Stats.DrawCalls++;
		}

		if (uint32_t vertexCount = (uint32_t)(s_Data->LineVertexPtr - s_Data->LineVertices))
		{
			size_t size = ((uint8_t*)s_Data->LineVertexPtr - (uint8_t*)s_Data->LineVertices);
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertices, size);

			Renderer::BindPipeline(s_Data->LineRendererPipeline.get());
			Renderer::GetCommandBuffer()->SetLineWidth(1.0f);
			Renderer::Submit(s_Data->LineVertexBuffer.get(), vertexCount);
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
		s_Data->MaxSamplerDestinations = Renderer::GetCapabilities().SamplerCapabilities.MaxPerStageSamplers;

		// Camera buffer
		{
			UniformBufferSpecification spec;
			spec.Size = sizeof(s_Data->CameraBufferData);
			spec.Binding = 0;
			s_Data->CameraUniformBuffer = UniformBuffer::Create(spec);
			s_Data->UniformBuffers = s_Data->CameraUniformBuffer.get();
		}

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

		s_Data->QuadIndexBuffer = IndexBuffer::Create(indices, Renderer2DData::MaxIndices);
		delete[] indices;

		s_Data->QuadVertices = new QuadVertex[Renderer2DData::MaxVertices];

		uint32_t textureColor = 0xffffffff;
		Texture2DSpecification spec = { 1, 1, 1, 1,
			ColorFormat::RGBA8,
			SamplerFilterMode::Nearest, SamplerFilterMode::Nearest, SamplerAddressMode::Repeat
		};

		s_Data->DefaultTexture = Texture2D::Create(spec);
		s_Data->DefaultTexture->SetData(&textureColor, sizeof(textureColor));
		s_Data->Textures = new const Texture*[s_Data->MaxSamplerDestinations];

		std::memset(s_Data->Textures, 0, s_Data->MaxSamplerDestinations * sizeof(Texture*));
		s_Data->Textures[0] = s_Data->DefaultTexture.get();

		auto& coreDirectory = Application::Get()->GetOptions().CoreDir;
		auto spriteVertexSource = ReadSpirvFile((coreDirectory / "bin-spirv/Sprite.vert.spv").string());
		auto spriteFragmentSource = ReadSpirvFile((coreDirectory / "bin-spirv/Sprite.frag.spv").string());

		AutoRelease<int32_t> samplersDests = StackAlloc(s_Data->MaxSamplerDestinations * sizeof(int32_t));
		for (uint32_t i = 0; i < s_Data->MaxSamplerDestinations; i++)
			samplersDests[i] = i;

		s_Data->QuadShader = Shader::Create("defaultSprite", spriteVertexSource, spriteFragmentSource);
		s_Data->QuadShader->SetIntArray("u_Samplers", samplersDests, s_Data->MaxSamplerDestinations);

		{
			PipelineSpecification spec;
			spec.Layout             = s_QuadVertexLayout;
			spec.Topology           = PrimitiveTopology::TriangleList;
			spec.Samples            = Renderer::GetMultisampling();
			spec.RenderPass         = Renderer::GetGeometryPass();
			spec.Shader             = s_Data->QuadShader.get();
			spec.UniformBufferCount = 1;
			spec.UniformBuffers     = &s_Data->UniformBuffers;

			s_Data->QuadRendererPipeline = Pipeline::Create(spec);
		}
	}

	static void SetupLineRenderer()
	{
		QK_PROFILE_FUNCTION();

		s_Data->LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(LineVertex));
		s_Data->LineVertexBuffer->SetLayout(s_LineVertexLayout);

		s_Data->LineVertices = new LineVertex[Renderer2DData::MaxVertices];

		auto& coreDirectory = Application::Get()->GetOptions().CoreDir;
		auto lineVertexSource = ReadSpirvFile((coreDirectory / "bin-spirv/Line.vert.spv").string());
		auto lineFragmentSource = ReadSpirvFile((coreDirectory / "bin-spirv/Line.frag.spv").string());

		s_Data->LineShader = Shader::Create("defaultLine", lineVertexSource, lineFragmentSource);

		{
			PipelineSpecification spec;
			spec.Layout             = s_LineVertexLayout;
			spec.Topology           = PrimitiveTopology::LineList;
			spec.Samples            = Renderer::GetMultisampling();
			spec.RenderPass         = Renderer::GetGeometryPass();
			spec.Shader             = s_Data->LineShader.get();
			spec.UniformBufferCount = 1;
			spec.UniformBuffers     = &s_Data->UniformBuffers;

			s_Data->LineRendererPipeline = Pipeline::Create(spec);
		}
	}
}

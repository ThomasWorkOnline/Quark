#include "qkpch.h"
#include "Renderer2D.h"
#include "GraphicsAPI.h"

#include "Quark/Core/Application.h"
#include "Quark/Renderer/Renderer.h"
#include "Quark/Filesystem/Filesystem.h"

#include <sstream>

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
		 v	 |         v/
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

		uint32_t MaxSamplers = 0;

		QuadVertex* QuadVertexPtr = nullptr;
		QuadVertex* QuadVertices  = nullptr;
		uint32_t QuadIndexCount   = 0;
		uint32_t QuadSamplerIndex = 1; // Next texture slot to be attached, 0 is reserved for default texture

		Scope<IndexBuffer> QuadIndexBuffer;

		Scope<Shader> QuadShader;
		Scope<VertexBuffer> QuadVertexBuffer;
		Scope<Pipeline> QuadRendererPipeline;

		QuadVertex* FontVertexPtr = nullptr;
		QuadVertex* FontVertices  = nullptr;
		uint32_t FontIndexCount   = 0;
		uint32_t FontSamplerIndex = 0;

		Scope<Shader> FontShader;
		Scope<VertexBuffer> TextVertexBuffer;
		Scope<Pipeline> TextRendererPipeline;

		LineVertex* LineVertexPtr = nullptr;
		LineVertex* LineVertices  = nullptr;

		Scope<Shader> LineShader;
		Scope<VertexBuffer> LineVertexBuffer;
		Scope<Pipeline> LineRendererPipeline;

		Scope<Texture2D> DefaultTexture;
		Texture2D** Textures = nullptr;
		Font** Fonts = nullptr;

		// Ensure std140 layout
		struct CameraData
		{
			Mat4f ViewProjection;
		};

		CameraData CameraBufferData{};
		Scope<UniformBuffer> CameraUniformBuffer;
	};

	Renderer2DStats Renderer2D::s_Stats;
	static Renderer2DData* s_Data = nullptr;

	static void SetupQuadRenderer();
	static void SetupFontRenderer();
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
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBufferData, sizeof(Renderer2DData::CameraBufferData));

		ResetStats();
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		PushBatch();
	}
	
	void Renderer2D::DrawSprite(Texture2D* texture, const Mat4f& transform)
	{
		DrawSprite(texture, s_TextureCoords, transform);
	}

	void Renderer2D::DrawSprite(Texture2D* texture, const Vec2f* texCoords, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

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
			s_Data->QuadVertexPtr->Position = transform * s_QuadVertexPositions[i];
			s_Data->QuadVertexPtr->TexCoord = texCoords[i];
			s_Data->QuadVertexPtr->Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			s_Data->QuadVertexPtr->TexIndex = textureIndex;

			s_Data->QuadVertexPtr++;
		}

		s_Data->QuadIndexCount += 6;
		s_Stats.QuadsDrawn++;
	}

	void Renderer2D::DrawSprite(const SubTexture2D& subTexture, const Mat4f& transform)
	{
		DrawSprite(subTexture.GetTexture(), subTexture.GetCoords(), transform);
	}

	void Renderer2D::DrawSprite(const Vec4f& color, const Mat4f& transform)
	{
		QK_ASSERT_RENDER_THREAD();

		// Check if buffer is full
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			PushBatch();
			StartBatch();
		}

		for (uint8_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexPtr->Position = transform * s_QuadVertexPositions[i];
			s_Data->QuadVertexPtr->TexCoord = { 0.0f, 0.0f };
			s_Data->QuadVertexPtr->Color = color;
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
		QK_ASSERT_RENDER_THREAD();

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
			s_Data->Fonts[textureIndex] = traits.FontStyle.get();
			s_Data->FontSamplerIndex++;
		}

		auto [x, y] = traits.GetOrigin();

		float atlasWidth = (float)traits.FontStyle->GetAtlasWidth();
		float atlasHeight = (float)traits.FontStyle->GetAtlasHeight();

		for (auto charcode : text)
		{
			auto& g = traits.FontStyle->GetGlyph(charcode);

			int32_t xpos = (x + g.Bearing.x);
			int32_t ypos = (-y - g.Bearing.y);
			int32_t w = g.Size.x;
			int32_t h = g.Size.y;
			float tx = (float)g.OffsetX / atlasWidth;

			x += (g.Advance.x >> 6);
			y += (g.Advance.y >> 6);

			if (!w || !h)
				continue;

			s_Data->FontVertexPtr->Position = Vec4f(xpos + w, -ypos, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx + g.Size.x / atlasWidth, 0.0f };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontVertexPtr->Position = Vec4f(xpos, -ypos, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx, 0.0f };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontVertexPtr->Position = Vec4f(xpos, -ypos - h, 0.0f, 1.0f);
			s_Data->FontVertexPtr->TexCoord = { tx, g.Size.y / atlasHeight };
			s_Data->FontVertexPtr->Color    = traits.Color;
			s_Data->FontVertexPtr->TexIndex = textureIndex;
			s_Data->FontVertexPtr++;

			s_Data->FontVertexPtr->Position = Vec4f(xpos + w, -ypos - h, 0.0f, 1.0f);
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

	void Renderer2D::SetViewport(uint32_t x, uint32_t y, uint32_t viewportWidth, uint32_t viewportHeight)
	{
		s_Data->QuadRendererPipeline->SetViewport(viewportWidth, viewportHeight);
		s_Data->TextRendererPipeline->SetViewport(viewportWidth, viewportHeight);
		s_Data->LineRendererPipeline->SetViewport(viewportWidth, viewportHeight);
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

			Renderer::BindPipeline(s_Data->QuadRendererPipeline.get());
			Renderer::Submit(s_Data->QuadVertexBuffer.get(), s_Data->QuadIndexBuffer.get(), s_Data->QuadIndexCount);
			s_Stats.DrawCalls++;
		}

		if (s_Data->FontIndexCount > 0)
		{
			size_t size = ((uint8_t*)s_Data->FontVertexPtr - (uint8_t*)s_Data->FontVertices);
			s_Data->TextVertexBuffer->SetData(s_Data->FontVertices, size);

			for (uint32_t i = 0; i < s_Data->FontSamplerIndex; i++)
				s_Data->Fonts[i]->Attach(i);

			Renderer::BindPipeline(s_Data->TextRendererPipeline.get());
			Renderer::Submit(s_Data->TextVertexBuffer.get(), s_Data->QuadIndexBuffer.get(), s_Data->FontIndexCount);
			s_Stats.DrawCalls++;
		}

		if (uint32_t vertexCount = (uint32_t)(s_Data->LineVertexPtr - s_Data->LineVertices))
		{
			size_t size = ((uint8_t*)s_Data->LineVertexPtr - (uint8_t*)s_Data->LineVertices);
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertices, size);

			Renderer::BindPipeline(s_Data->LineRendererPipeline.get());
			Renderer::DrawLines(s_Data->LineVertexBuffer.get(), vertexCount);
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
		s_Data->MaxSamplers = GraphicsAPI::Instance->GetCapabilities().TextureConstraints.MaxTextureSlots;
		QK_CORE_ASSERT(s_Data->MaxSamplers > 0, "Platform does not support texture samplers");

		// Camera
		s_Data->CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		SetupQuadRenderer();
		SetupFontRenderer();
		SetupLineRenderer();
	}

	void Renderer2D::Dispose()
	{
		QK_PROFILE_FUNCTION();

		if (!s_Data) return;

		delete[] s_Data->QuadVertices;
		delete[] s_Data->FontVertices;
		delete[] s_Data->LineVertices;
		delete[] s_Data->Textures;
		delete[] s_Data->Fonts;

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
		Texture2DSpecification spec = { 1, 1, 1,
			ColorDataFormat::RGBA8,
			TextureFilteringMode::Nearest, TextureFilteringMode::Nearest, TextureTilingMode::Repeat
		};

		s_Data->DefaultTexture = Texture2D::Create(spec);
		s_Data->DefaultTexture->SetData(&textureColor, sizeof(textureColor));

		s_Data->Textures = new Texture2D*[s_Data->MaxSamplers];

		std::memset(s_Data->Textures, 0, s_Data->MaxSamplers * sizeof(Texture2D*));
		s_Data->Textures[0] = s_Data->DefaultTexture.get();

		auto& assetDir = Application::Get()->GetOptions().CoreAssetDir;
		std::string spriteVertexSource = Filesystem::ReadTextFile((assetDir / "bin-spirv/textured_sprite.vert.spv").string());
		std::string spriteFragmentSource = Filesystem::ReadTextFile((assetDir / "bin-spirv/textured_sprite.frag.spv").string());

		s_Data->QuadShader = Shader::Create("defaultSprite", spriteVertexSource, spriteFragmentSource);

		{
			PipelineSpecification spec;
			spec.ViewportWidth = Renderer::GetViewportExtent().Width;
			spec.ViewportHeight = Renderer::GetViewportExtent().Height;
			spec.CameraUniformBufferSize = sizeof(Renderer2DData::CameraBufferData);
			spec.Layout = s_QuadVertexLayout;
			spec.RenderPass = Renderer::GetGeometryPass();
			spec.Shader = s_Data->QuadShader.get();

			s_Data->QuadRendererPipeline = Pipeline::Create(spec);
		}
	}

	static void SetupFontRenderer()
	{
		QK_PROFILE_FUNCTION();

		s_Data->TextVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
		s_Data->TextVertexBuffer->SetLayout(s_QuadVertexLayout);

		s_Data->FontVertices = new QuadVertex[Renderer2DData::MaxVertices];
		s_Data->Fonts = new Font*[s_Data->MaxSamplers];
		std::memset(s_Data->Fonts, 0, s_Data->MaxSamplers * sizeof(Font*));

		auto& assetDir = Application::Get()->GetOptions().CoreAssetDir;
		std::string textVertexSource = Filesystem::ReadTextFile((assetDir / "bin-spirv/text.vert.spv").string());
		std::string textFragmentSource = Filesystem::ReadTextFile((assetDir / "bin-spirv/text.frag.spv").string());

		s_Data->FontShader = Shader::Create("defaultFont", textVertexSource, textFragmentSource);

		{
			PipelineSpecification spec;
			spec.ViewportWidth = Renderer::GetViewportExtent().Width;
			spec.ViewportHeight = Renderer::GetViewportExtent().Height;
			spec.CameraUniformBufferSize = sizeof(Renderer2DData::CameraBufferData);
			spec.Layout = s_QuadVertexLayout;
			spec.RenderPass = Renderer::GetGeometryPass();
			spec.Shader = s_Data->FontShader.get();

			s_Data->TextRendererPipeline = Pipeline::Create(spec);
		}
	}

	static void SetupLineRenderer()
	{
		QK_PROFILE_FUNCTION();

		s_Data->LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(LineVertex));
		s_Data->LineVertexBuffer->SetLayout(s_LineVertexLayout);

		s_Data->LineVertices = new LineVertex[Renderer2DData::MaxVertices];

		auto& assetDir = Application::Get()->GetOptions().CoreAssetDir;
		std::string lineVertexSource = Filesystem::ReadTextFile((assetDir / "bin-spirv/line.vert.spv").string());
		std::string lineFragmentSource = Filesystem::ReadTextFile((assetDir / "bin-spirv/line.frag.spv").string());

		s_Data->LineShader = Shader::Create("defaultLine", lineVertexSource, lineFragmentSource);

		{
			PipelineSpecification spec;
			spec.ViewportWidth = Renderer::GetViewportExtent().Width;
			spec.ViewportHeight = Renderer::GetViewportExtent().Height;
			spec.CameraUniformBufferSize = sizeof(Renderer2DData::CameraBufferData);
			spec.Layout = s_LineVertexLayout;
			spec.RenderPass = Renderer::GetGeometryPass();
			spec.Shader = s_Data->LineShader.get();

			s_Data->LineRendererPipeline = Pipeline::Create(spec);
		}
	}
}

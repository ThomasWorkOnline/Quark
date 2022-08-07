#include "TextureArrayTest.h"

struct SpriteVertex
{
	Vec3f Position;
	Vec2f TexCoord;
	uint32_t TexIndex;
};

TextureArrayTest::TextureArrayTest()
{
	Image texture1 = Image("assets/textures/Example1_BasicRendering.png");

	Texture2DArraySpecification spec;
	spec.DataFormat = ColorDataFormat::RGBA8_SRGB;
	spec.Width = texture1.Width();
	spec.Height = texture1.Height();
	spec.Layers = 2;
	spec.RenderModes.MinFilteringMode = TextureFilteringMode::NearestMipmapLinear;
	spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;

	m_TextureArray = Texture2DArray::Create(spec);
	m_TextureArray->SetData(texture1.Data(), texture1.Size(), 0);
	m_TextureArray->SetData(texture1.Data(), texture1.Size(), 1);
	m_TextureArray->GenerateMipmaps();

	m_Shader = Shader::Create("assets/shaders/textureArray.glsl");
	m_Shader->Attach();
	m_Shader->SetInt("u_Sampler", 0);

	static constexpr SpriteVertex vertices[] = {
		{ { 0.0f, 0.0f, 1.0f },  { 0.0f, 1.0f }, 0 },
		{ { 1.0f, 0.0f, 1.0f },  { 1.0f, 1.0f }, 0 },
		{ { 1.0f, 1.0f, 1.0f },  { 1.0f, 0.0f }, 1 },
		{ { 0.0f, 1.0f, 1.0f },  { 0.0f, 0.0f }, 0 }
	};

	static constexpr uint32_t indices[] = {
		0, 3, 2,
		0, 2, 1
	};

	m_VertexArray = VertexArray::Create();
	m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	m_VertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Int,    "a_TexIndex" }
		});
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);

	auto window = GetWindow();

	m_Camera.SetPerspective(70.0f);
	m_Camera.Resize(window->GetWidth(), window->GetHeight());
}

void TextureArrayTest::OnUpdate(Timestep elapsedTime)
{
	m_ModelTransform = glm::rotate(m_ModelTransform, elapsedTime * 0.1f, Vec3f(1.0f, 0.0f, 0.0f));
}

void TextureArrayTest::OnRender()
{
	static constexpr Vec4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Renderer2D::BeginScene(m_Camera.GetProjection(), m_CameraTransform);
	Renderer2D::DrawLine(Vec3f(-1, -1, 0), Vec3f(1, 1, 0), color, color);
	Renderer2D::EndScene();

	m_TextureArray->Attach();
	//Renderer::Submit(m_Shader, m_VertexArray, m_ModelTransform);
}

void TextureArrayTest::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(TextureArrayTest::OnWindowResized));
}

bool TextureArrayTest::OnWindowResized(WindowResizedEvent& e)
{
	m_Camera.Resize(e.GetWidth(), e.GetHeight());
	return false;
}

#include "TextureArrayTest.h"

struct SpriteVertex
{
	Vec3f Position;
	Vec2f TexCoord;
	uint32_t TexIndex;
};

TextureArrayTest::TextureArrayTest(const ApplicationOptions& options)
	: Application(options)
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
	m_TextureArray->SetData(texture1.GetData(), texture1.GetMetadata().Size, 0);
	m_TextureArray->SetData(texture1.GetData(), texture1.GetMetadata().Size, 1);
	m_TextureArray->GenerateMipmaps();

	m_Shader = Shader::Create("assets/shaders/textureArray.glsl");
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

	m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	m_VertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Int,    "a_TexIndex" }
	});

	m_IndexBuffer = IndexBuffer::Create(indices, sizeof_array(indices));

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
	static constexpr Vec4f color = { 1.0f, 1.0f, 0.0f, 1.0f };
	Renderer2D::BeginScene(m_Camera.GetProjection(), m_CameraTransform);

	Renderer2D::DrawSprite(color);
	Renderer2D::DrawLine(Vec3f(-100.0f, -100.0f, 1.0f), Vec3f(100.0f, 100.0f, 1.0f), color, color);

	Renderer2D::EndScene();
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

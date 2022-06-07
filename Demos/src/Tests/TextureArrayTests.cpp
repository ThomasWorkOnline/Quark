#include "TextureArrayTest.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	uint32_t TexIndex;
};

TextureArrayTest::TextureArrayTest()
{
	Ref<Image> texture1 = Image::Create("assets/textures/Example1_BasicRendering.png");

	TextureArraySpecification spec;
	spec.DataFormat     = TextureDataFormat::RGBA;
	spec.InternalFormat = TextureInternalFormat::SRGBA8;
	spec.Width = texture1->Width();
	spec.Height = texture1->Height();
	spec.Layers = 2;
	spec.RenderModes.MinFilteringMode = TextureFilteringMode::NearestMipmapLinear;
	spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;


	m_TextureArray = Texture2DArray::Create(spec);
	m_TextureArray->SetData(texture1->Data(), texture1->Size(), 0);
	m_TextureArray->SetData(texture1->Data(), texture1->Size(), 1);
	m_TextureArray->GenerateMipmaps();

	m_Shader = Shader::Create("assets/shaders/textureArray.glsl");
	m_Shader->Attach();
	m_Shader->SetInt("u_Sampler", 0);

	static constexpr Vertex vertices[] = {
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

	m_Camera.SetPerspective(70.0f);
}

void TextureArrayTest::OnUpdate(Timestep elapsedTime)
{
	m_Transform = glm::rotate(m_Transform, elapsedTime * 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void TextureArrayTest::OnRender()
{
	Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

	static constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Renderer2D::DrawLine(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0), color, color);

	m_Shader->Attach();
	m_Shader->SetMat4("u_Model", m_Transform);

	m_TextureArray->Attach();
	RenderCommand::DrawIndexed(m_VertexArray);

	Renderer::EndScene();
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

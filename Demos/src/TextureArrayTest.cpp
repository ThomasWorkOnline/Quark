#include "TextureArrayTest.h"

TextureArrayTest::TextureArrayTest(const ApplicationSpecification& spec)
	: Application(spec)
{
	Image texture1 = Image("assets/Textures/Example1_BasicRendering.png");

	Texture2DArraySpecification textureSpec;
	textureSpec.DataFormat = ColorFormat::RGBA8SRGB;
	textureSpec.Width = texture1.Width();
	textureSpec.Height = texture1.Height();
	textureSpec.Layers = 2;

	m_TextureArray = Texture2DArray::Create(textureSpec);
	m_TextureArray->SetData(texture1.GetData(), texture1.GetMetadata().Size, 0);
	m_TextureArray->SetData(texture1.GetData(), texture1.GetMetadata().Size, 1);
	m_TextureArray->GenerateMipmaps();

	auto* window = GetWindow();
	m_Camera.SetPerspective(70.0f);
	m_Camera.Resize((float)window->GetWidth(), (float)window->GetHeight());
}

void TextureArrayTest::OnUpdate(Timestep elapsedTime)
{
	m_ModelTransform = glm::rotate(m_ModelTransform, elapsedTime * 0.1f, Vec3f(1.0f, 0.0f, 0.0f));
}

void TextureArrayTest::OnRender()
{
	static constexpr Vec4f color = { 1.0f, 1.0f, 0.0f, 1.0f };

	Renderer2D::BeginScene(m_Camera.GetProjection(), m_CameraTransform);

	Renderer2D::DrawQuad(color);
	Renderer2D::DrawLine(Vec3f(-100.0f, -100.0f, 1.0f), Vec3f(100.0f, 100.0f, 1.0f), color, color);

	Renderer2D::EndScene();
}

void TextureArrayTest::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(TextureArrayTest::OnWindowResized));
}

void TextureArrayTest::OnWindowResized(WindowResizedEvent& e)
{
	m_Camera.Resize((float)e.GetWidth(), (float)e.GetHeight());
}

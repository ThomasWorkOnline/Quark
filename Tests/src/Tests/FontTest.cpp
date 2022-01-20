#include "FontTest.h"

FontTest::FontTest()
{
	TextureArraySpecification spec;
	spec.Width = 48;
	spec.Height = 48;
	spec.Layers = 1;
	spec.DataFormat = TextureDataFormat::RGBA8;
	spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;

	m_Font = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 0, 128);
	m_Font2 = m_Library.Load("agency-regular", "assets/fonts/ANTQUAI.TTF", 0, 64);

	m_Text = Text("Some sample text!", m_Font, m_Color, 1.0f, 1.0f, HorizontalTextAlignment::Left);
	m_Transform2 = glm::translate(m_Transform2, glm::vec3(-0.4f, -0.3f, 0.0f));
	m_Texture = Texture2D::Create("assets/textures/sprite_sheet.png", spec.RenderModes);

	Image image("assets/textures/sprite_sheet.png", true);

	m_TextureArray = Texture2DArray::Create(spec);
	m_TextureArray->SetData(image.Data(), image.Size(), 0);
}

void FontTest::OnUpdate(float elapsedTime)
{
	m_Camera.OnUpdate();

	static float accumTime = elapsedTime;
	accumTime += elapsedTime;

	{
		Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

		constexpr glm::vec4 colorStart = { 0.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec4 colorEnd = { 1.0f, 0.0f, 0.0f, 1.0f };

		Renderer::DrawLine({ 0, 0, 1 }, { 1, 0.5, 1 }, colorStart, colorEnd);

#if 1
		Renderer::DrawSprite(m_Texture, m_Transform3);

		RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);
		Renderer::DrawText(m_Text, m_Transform);
		Renderer::DrawText(m_Font2, "Hi there!", glm::vec4(1.0f), glm::vec2(1.0f));
		RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
#endif

		Renderer::EndScene();
	}
}

void FontTest::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(FontTest::OnWindowResized));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(FontTest::OnKeyPressed));
}

bool FontTest::OnWindowResized(WindowResizedEvent& e)
{
	m_Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());
	return false;
}

bool FontTest::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case KeyCode::F11:
	{
		auto& window = GetWindow();
		bool fullscreen = window.IsFullscreen();
		window.SetFullScreen(!fullscreen);
		break;
	}
	}
	return false;
}

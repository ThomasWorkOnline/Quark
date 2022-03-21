#include "MainLayer.h"

MainLayer::MainLayer()
{
	TextureArraySpecification spec;
	spec.Width = 48;
	spec.Height = 48;
	spec.Layers = 1;
	spec.DataFormat = TextureDataFormat::RGBA8;
	spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;

	m_Font1 = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 0, 128);
	m_Font2 = m_Library.Load("agency-regular", "assets/fonts/ANTQUAI.TTF", 0, 64);

	m_Text = Text("Some sample text!", m_Font1, m_Color1, 1.0f, 1.0f, HorizontalTextAlignment::Left);
	m_Transform2 = glm::translate(m_Transform2, glm::vec3(-0.4f, -0.3f, 0.0f));
	m_Texture = Texture2D::Create("assets/textures/sprite_sheet.png", spec.RenderModes);
}

void MainLayer::OnUpdate(float elapsedTime)
{
	Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

	static constexpr glm::vec4 colorStart = { 0.0f, 1.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 colorEnd = { 1.0f, 0.0f, 0.0f, 1.0f };

	Renderer::DrawLine({ 0, 0, 1 }, { 1, 0.5, 1 }, colorStart, colorEnd);

#if 1
	Renderer::DrawSprite(m_Texture, m_Transform3);

	RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);
	Renderer::DrawText(m_Text, m_Transform1);
	Renderer::DrawText(m_Font2, "Hi there!", m_Color2, glm::vec2(1.0f), glm::vec2(0.0f), m_Transform2);
	RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
#endif

	Renderer::EndScene();
}

void MainLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(MainLayer::OnWindowResized));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(MainLayer::OnKeyPressed));
}

bool MainLayer::OnWindowResized(WindowResizedEvent& e)
{
	m_Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());
	return false;
}

bool MainLayer::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::F11:
		{
			auto& window = Application::Get().GetWindow();
			bool fullscreen = window.IsFullscreen();
			window.SetFullScreen(!fullscreen);
			break;
		}
	}
	return false;
}

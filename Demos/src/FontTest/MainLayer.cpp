#include "MainLayer.h"

MainLayer::MainLayer()
{
	TextureArraySpecification spec;
	spec.Width = 48;
	spec.Height = 48;
	spec.Layers = 1;
	spec.DataFormat = TextureDataFormat::RGBA;
	spec.InternalFormat = TextureInternalFormat::RGBA8;
	spec.RenderModes.MinFilteringMode = TextureFilteringMode::Linear;
	spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;

	m_Font1 = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 48);
	m_Font2 = m_Library.Load("agency-regular", "assets/fonts/ANTQUAI.TTF", 64);

	m_Text  = Text("Hello quad", m_Font1, m_Color1, HorizontalTextAlignment::Left, VerticalTextAlignment::Center);
	m_Text2 = Text("Hello quad", m_Font2, m_Color1, HorizontalTextAlignment::Right, VerticalTextAlignment::Bottom);
	m_Input = TextInput(m_Font1, HorizontalTextAlignment::Center);

	TextureDescriptor descriptor;
	descriptor.SRGB = true;
	m_Texture = Texture2D::Create("assets/textures/pbr/streaked-metal/normal-dx.png", descriptor);

	auto& window = Application::Get().GetWindow();
	float width  = window.GetWidth();
	float height = window.GetHeight();

	m_Camera.SetProjection(-width / 2, width / 2, -height / 2, height / 2);
}

void MainLayer::OnUpdate(Timestep elapsedTime)
{
	Renderer::BeginScene(m_Camera.GetProjection(), glm::mat4(1.0f));
	Renderer2D::DrawTextInput(m_Input);
	Renderer::EndScene();
}

void MainLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(MainLayer::OnWindowResized));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(MainLayer::OnKeyPressed));

	if (!e.Handled)
		m_Input.OnEvent(e);
}

bool MainLayer::OnWindowResized(WindowResizedEvent& e)
{
	float width = e.GetWidth();
	float height = e.GetHeight();

	m_Camera.SetProjection(-width / 2, width / 2, -height / 2, height / 2);
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
		case KeyCode::Left:
			m_Text.SetHorizontalAlignment(HorizontalTextAlignment::Left);
			break;
		case KeyCode::Right:
			m_Text.SetHorizontalAlignment(HorizontalTextAlignment::Right);
			break;
		case KeyCode::Up:
			m_Text.SetVerticalAlignment(VerticalTextAlignment::Top);
			break;
		case KeyCode::Down:
			m_Text.SetVerticalAlignment(VerticalTextAlignment::Bottom);
			break;
		case KeyCode::N:
			m_Text.SetText("New text is now longer!");
			break;
		case KeyCode::F:
			if (*m_Text.GetRenderTraits().FontStyle == *m_Font1)
				m_Text.SetFont(m_Font2);
			else
				m_Text.SetFont(m_Font1);
			break;
	}
	return false;
}

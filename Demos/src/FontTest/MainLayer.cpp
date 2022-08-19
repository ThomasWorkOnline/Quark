#include "MainLayer.h"

MainLayer::MainLayer(Application* app) : Layer(app)
{
	m_Texture = Texture2D::Create("assets/textures/pbr/streaked-metal/normal-dx.png");

	m_Font1 = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 48);
	m_Font2 = m_Library.Load("agency-regular", "assets/fonts/ANTQUAI.TTF", 64);

	{
		static constexpr Vec4f color1 = { 1.0f, 1.0f, 0.0f, 1.0f };
		static constexpr Vec4f color2 = { 1.0f, 0.0f, 1.0f, 0.5f };

		TextStyle style1;
		style1.HorizontalAlign = HorizontalTextAlignment::Left;
		style1.VerticalAlign = VerticalTextAlignment::Center;
		style1.Font = m_Font1;
		style1.Color = color1;

		TextStyle style2;
		style1.HorizontalAlign = HorizontalTextAlignment::Right;
		style1.VerticalAlign = VerticalTextAlignment::Bottom;
		style2.Font = m_Font2;
		style2.Color = color2;

		m_Text1 = Text("Hello quad", style1);
		m_Text2 = Text("Hello quad", style2);
		m_Input = TextInput(style1);
	}

	auto window = GetApplication()->GetWindow();
	m_Camera.SetOrthographic((float)window->GetWidth());
	m_Camera.Resize(window->GetWidth(), window->GetHeight());
}

void MainLayer::OnUpdate(Timestep elapsedTime)
{
}

void MainLayer::OnRender()
{
	Renderer2D::BeginScene(m_Camera.GetProjection(), Mat4f(1.0f));
	Renderer2D::DrawText(m_Input);
	Renderer2D::EndScene();
}

void MainLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(MainLayer::OnWindowResized));
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(MainLayer::OnKeyPressed));

	if (!e.Handled)
		m_Input.OnEvent(e);
}

void MainLayer::OnWindowResized(WindowResizedEvent& e)
{
	m_Camera.Resize(e.GetWidth(), e.GetHeight());
}

void MainLayer::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case KeyCode::F11:
		{
			auto window = GetApplication()->GetWindow();
			bool fullscreen = window->IsFullscreen();
			window->SetFullScreen(!fullscreen);
			break;
		}
		case KeyCode::Left:
		{
			m_Text1.SetHorizontalAlignment(HorizontalTextAlignment::Left);
			break;
		}
		case KeyCode::Right:
		{
			m_Text1.SetHorizontalAlignment(HorizontalTextAlignment::Right);
			break;
		}
		case KeyCode::Up:
		{
			m_Text1.SetVerticalAlignment(VerticalTextAlignment::Top);
			break;
		}
		case KeyCode::Down:
		{
			m_Text1.SetVerticalAlignment(VerticalTextAlignment::Bottom);
			break;
		}
		case KeyCode::N:
		{
			m_Text1.SetValue("New text is now longer!");
			break;
		} 
		case KeyCode::F:
		{
			if (*m_Text1.GetStyle().Font == *m_Font1)
				m_Text1.SetFont(m_Font2);
			else
				m_Text1.SetFont(m_Font1);

			break;
		} 
	}
}

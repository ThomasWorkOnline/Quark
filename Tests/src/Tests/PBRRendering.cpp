#include "PBRRendering.h"

PBRRendering::PBRRendering()
{
	m_Player = m_Scene.CreateEntity();
	m_Player.AddComponent<Transform3DComponent>().Position = { 0.0f, 0.0f, -2.0f };
	m_Player.AddComponent<PhysicsComponent>().Friction = 4.0f;
	m_Player.AddComponent<PerspectiveCameraComponent>((float)GetWindow().GetWidth() / GetWindow().GetHeight(), 70.0f);
	
	m_Controller = { m_Player };

	m_TextureModes.MinFilteringMode = TextureFilteringMode::Linear;
	m_TextureModes.MagFilteringMode = TextureFilteringMode::Nearest;
	m_Texture = Texture2D::Create("assets/textures/sprite_sheet.png", m_TextureModes);
	m_Shader = Shader::Create("assets/shaders/default3D.glsl");
	m_Cube.GenerateUnitCube();
}

void PBRRendering::OnUpdate(Timestep elapsedTime)
{
	m_Controller.OnUpdate(elapsedTime);
	m_Scene.OnUpdate(elapsedTime);
	
	const auto& camera = m_Player.GetComponent<PerspectiveCameraComponent>().Camera;
	const auto& transform = m_Player.GetComponent<Transform3DComponent>();
	Renderer::BeginScene(camera.GetProjection(), transform);

	Renderer::DrawSprite(m_Texture);

	Renderer::Submit(m_Shader, m_Texture, m_Cube.GetVertexArray());

	Renderer::EndScene();
}

void PBRRendering::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(PBRRendering::OnKeyPressed));
	dispatcher.Dispatch<MouseMovedEvent>(ATTACH_EVENT_FN(PBRRendering::OnMouseMoved));
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(PBRRendering::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(ATTACH_EVENT_FN(PBRRendering::OnMouseButtonReleased));

	if (!e.Handled)
		m_Controller.OnEvent(e);
}

bool PBRRendering::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case KeyCode::Escape:
		auto& window = GetWindow();
		window.DisableCursor();
		break;
	}

	return false;
}

bool PBRRendering::OnMouseMoved(MouseMovedEvent& e)
{
	if (!GetWindow().IsCursorEnabled())
		return true;

	return false;
}

bool PBRRendering::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	GetWindow().EnableCursor();
	return false;
}

bool PBRRendering::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	return false;
}

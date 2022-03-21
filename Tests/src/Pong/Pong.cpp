#include "Pong.h"

static void* s_InitialAddress = nullptr;

Pong::Pong()
	: Application(ApplicationOptions(1280, 720, "Pong"))
{
	RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });

	auto& window = GetWindow();
	float aspectRatio = (float)window.GetWidth() / window.GetHeight();

	m_Entity = m_Scene.CreateEntity();
	m_Entity.AddComponent<Transform3DComponent>();
	m_Entity.AddComponent<PhysicsComponent>().Friction = 4.0f;
	m_Entity.AddComponent<PerspectiveCameraComponent>(aspectRatio, 70.0f);

	m_Controller = { m_Entity };
	m_BallTransform.Position = glm::vec3(0.0f, 0.0f, 10.0f);

	m_Shader = Shader::Create("assets/shaders/default3D.glsl");
	s_InitialAddress = m_Shader.get();

	FaceOff();
}

void Pong::OnUpdate(float elapsedTime)
{
	m_Controller.OnUpdate(elapsedTime);
	m_Scene.OnUpdate(elapsedTime);

	m_BallTransform.Position.x += m_Speed * m_CoeffX * elapsedTime;
	m_BallTransform.Position.y += m_Speed * m_CoeffY * elapsedTime;

	ProcessCollision();
	ProcessCollisionResolution();

	const auto& transform = m_Entity.GetComponent<Transform3DComponent>();

	glm::mat4 projection = m_Entity.GetComponent<PerspectiveCameraComponent>().Camera.GetProjection();
	glm::mat4 rotate = glm::toMat4(transform.Orientation);
	glm::mat4 view = glm::translate(rotate, -transform.Position);

	Renderer::BeginScene(projection, view);
	Renderer::Submit(m_Shader, m_Ball.GetVertexArray(), m_BallTransform);
	Renderer::EndScene();
}

void Pong::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(Pong::OnKeyPressed));
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(Pong::OnMouseButtonPressed));

	e.Handled = e.IsInCategory(EventCategoryInput) && !GetWindow().IsFocused();

	if (!e.Handled)
		m_Controller.OnEvent(e);
}

void Pong::FaceOff()
{
	Random<float> random;
	random.Seed(time(nullptr));
	m_CoeffX = roundf(random.Next()) * 2.0f - 1.0f;
	m_CoeffY = roundf(random.Next()) * 2.0f - 1.0f;
}

void Pong::ProcessCollision()
{
	auto& x = m_BallTransform.Position.x;
	auto& y = m_BallTransform.Position.y;

	m_CollidesX = (x < -7 || x > 7);
	m_CollidesY = (y < -5 || y > 5);
}

void Pong::ProcessCollisionResolution()
{
	if (m_CollidesX)
	{
		m_CoeffX = -m_CoeffX;
		AudioEngine::PlaySound("assets/sounds/break_dirt.mp3");
	}

	if (m_CollidesY)
	{
		m_CoeffY = -m_CoeffY;
		AudioEngine::PlaySound("assets/sounds/break_dirt.mp3");
	}
}

bool Pong::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
	case MouseCode::ButtonLeft:
		GetWindow().DisableCursor();
		break;
	}
	return false;
}

bool Pong::OnKeyPressed(KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case Key::Escape:
		GetWindow().EnableCursor();
		break;
	}
	return false;
}

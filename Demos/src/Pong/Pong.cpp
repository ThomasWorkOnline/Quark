#include "Pong.h"

Pong::Pong()
	: Application(ApplicationOptions(1280, 720, "Pong"))
{
	auto& window = GetWindow();

	m_Entity = m_Scene.CreateEntity();
	m_Entity.AddComponent<Transform3DComponent>();
	m_Entity.AddComponent<PhysicsComponent>().Friction = 4.0f;
	auto& camera = m_Entity.AddComponent<CameraComponent>().Camera;
	camera.SetPerspective(70.0f);
	camera.Resize(window.GetWidth(), window.GetHeight());

	m_BallTransform.Position = glm::vec3(0.0f, 0.0f, 10.0f);

	m_Shader = Shader::Create("assets/shaders/PBR.glsl");
	m_Ball = Mesh::LoadFromFile("assets/meshes/sphere.obj");

	m_AudioBuffer = AudioBuffer::Create("assets/sounds/Down V1.2.wav");
	m_AudioSource = AudioSource::Create();
	m_AudioSource->Attach(m_AudioBuffer->GetBufferID());

	m_AudioSource->Play();
	m_AudioPlaying = true;

	FaceOff();
}

void Pong::OnUpdate(Timestep elapsedTime)
{
	m_Scene.OnUpdate(elapsedTime);

#if 0
	m_BallTransform.Position.x += m_Speed * m_CoeffX * elapsedTime;
	m_BallTransform.Position.y += m_Speed * m_CoeffY * elapsedTime;

	ProcessCollision();
	ProcessCollisionResolution();
#endif
}

void Pong::OnRender()
{
	const auto& camera = m_Entity.GetComponent<CameraComponent>().Camera;
	const auto& transform = m_Entity.GetComponent<Transform3DComponent>();

	//Renderer::BeginScene(camera, transform);
	//Renderer::Submit(m_Shader, m_Ball.GetVertexArray(), m_BallTransform);
	//Renderer::EndScene();
}

void Pong::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(OnMouseButtonPressed));

	e.Handled = e.IsInCategory(EventCategory::Input) && GetWindow().IsCursorEnabled();
}

void Pong::FaceOff()
{
	Random<float> random;
	random.Seed((uint32_t)time(nullptr));
	m_CoeffX = glm::round(random.Next()) * 2.0f - 1.0f;
	m_CoeffY = glm::round(random.Next()) * 2.0f - 1.0f;
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
		//AudioEngine::PlaySound("assets/sounds/break_dirt.mp3");
	}

	if (m_CollidesY)
	{
		m_CoeffY = -m_CoeffY;
		//AudioEngine::PlaySound("assets/sounds/break_dirt.mp3");
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
		case KeyCode::Escape:
			GetWindow().EnableCursor();
			break;
		case KeyCode::Enter:
		{
			if (m_AudioPlaying)
				m_AudioSource->Pause();
			else 
				m_AudioSource->Play();
			m_AudioPlaying = !m_AudioPlaying;
			break;
		}
	}
	return false;
}

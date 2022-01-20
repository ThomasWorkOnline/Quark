#pragma once

#include <Quark.h>

using namespace Quark;

class Pong : public Application
{
public:
	Pong()
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

		FaceOff();
	}

	void OnUpdate(float elapsedTime) override
	{
		m_Controller.OnUpdate(elapsedTime);
		m_Scene.OnUpdate(elapsedTime);

		// Positions
		ProcessPositions(elapsedTime);

		// Tester collision
		ProcessCollision();

		// Resoudre collision
		ProcessCollisionResolution();

		RenderScene();
	}

	void OnEvent(Event& e) override
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ATTACH_EVENT_FN(Pong::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ATTACH_EVENT_FN(Pong::OnMouseButtonPressed));

		e.Handled = e.IsInCategory(EventCategoryInput) && !GetWindow().IsFocused();

		if (!e.Handled)
			m_Controller.OnEvent(e);
	}

private:
	void FaceOff()
	{
		Random random;
		random.Seed(time(nullptr));
		m_CoeffX = roundf(random.Next()) * 2.0f - 1.0f;
		m_CoeffY = roundf(random.Next()) * 2.0f - 1.0f;
	}

	void ProcessPositions(float elapsedTime)
	{
		m_BallTransform.Position.x += m_Speed * m_CoeffX * elapsedTime;
		m_BallTransform.Position.y += m_Speed * m_CoeffY * elapsedTime;
	}

	void ProcessCollision()
	{
		float& x = m_BallTransform.Position.x;
		float& y = m_BallTransform.Position.y;

		m_CollidesX = (x < -7 || x > 7);
		m_CollidesY = (y < -5 || y > 5);
	}

	void ProcessCollisionResolution()
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

	void RenderScene()
	{
		const auto& transform = m_Entity.GetComponent<Transform3DComponent>();

		glm::mat4 projection = m_Entity.GetComponent<PerspectiveCameraComponent>().Camera.GetProjection();
		glm::mat4 rotate = glm::toMat4(transform.Orientation);
		glm::mat4 view = glm::translate(rotate, -transform.Position);
		Renderer::BeginScene(projection, view);

		Renderer::Submit(m_Shader, m_Ball.GetVertexArray(), m_BallTransform);

		Renderer::EndScene();
	}

	bool OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case MouseCode::ButtonLeft:
			GetWindow().DisableCursor();
			break;
		}

		return false;
	}

	bool OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Key::Escape:
			GetWindow().EnableCursor();
			break;
		}

		return false;
	}

private:
	Scene m_Scene;
	Entity m_Entity;
	PerspectiveCameraController m_Controller;

	Ref<Shader> m_Shader = Shader::Create("assets/shaders/default3D.glsl");
	Mesh m_Ball = Mesh("assets/meshes/arrow.obj");
	Transform3DComponent m_BallTransform = Transform3DComponent();

	float m_Speed = 1.0f;
	float m_CoeffX = 0.0f, m_CoeffY = 0.0f;

	bool m_CollidesX = false, m_CollidesY = false;
};

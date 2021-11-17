#pragma once

#include <Quark.h>

class Pong : public Quark::Application
{
public:
	Pong()
		: Quark::Application(Quark::ApplicationOptions(1280, 720, "Pong"))
	{
		Quark::RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });

		auto& window = GetWindow();
		window.Select();
		float aspectRatio = (float)window.GetWidth() / window.GetHeight();

		{
			m_Entity = m_Scene.CreateEntity();

			m_Entity.AddComponent<Quark::Transform3DComponent>();
			m_Entity.AddComponent<Quark::PhysicsComponent>().Friction = 4.0f;
			m_Entity.AddComponent<Quark::PerspectiveCameraComponent>(aspectRatio, 70.0f);

			m_Controller = { m_Entity };
		}

		{
			m_BallTransform.Position = glm::vec3(0.0f, 0.0f, 10.0f);
		}

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

	void OnEvent(Quark::Event& e) override
	{
		Quark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(Pong::OnKeyPressed));
		dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(Pong::OnMouseButtonPressed));

		e.Handled = e.IsInCategory(EventCategoryInput) && !GetWindow().IsSelected();

		if (!e.Handled)
			m_Controller.OnEvent(e);
	}

private:
	void FaceOff()
	{
		Quark::Random random;
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
			Quark::AudioEngine::PlaySound("assets/sounds/break_dirt.mp3");
		}

		if (m_CollidesY)
		{
			m_CoeffY = -m_CoeffY;
			Quark::AudioEngine::PlaySound("assets/sounds/break_dirt.mp3");
		}
	}

	void RenderScene()
	{
		const auto& transform = m_Entity.GetComponent<Quark::Transform3DComponent>();

		glm::mat4 projection = m_Entity.GetComponent<Quark::PerspectiveCameraComponent>().Camera.GetProjection();
		glm::mat4 rotate = glm::toMat4(transform.Orientation);
		glm::mat4 view = glm::translate(rotate, -transform.Position);
		Quark::Renderer::BeginScene(projection, view);

		Quark::Renderer::Submit(m_Shader, m_Ball.GetVertexArray(), m_BallTransform);

		Quark::Renderer::EndScene();
	}

	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Quark::MouseCode::ButtonLeft:
			GetWindow().Select();
			break;
		}

		return false;
	}

	bool OnKeyPressed(Quark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Quark::Key::Escape:
			GetWindow().Deselect();
			break;
		}

		return false;
	}

private:
	Quark::Scene m_Scene;
	Quark::Entity m_Entity;
	Quark::PerspectiveCameraController m_Controller;

	Quark::Ref<Quark::Shader> m_Shader = Quark::Shader::Create("assets/shaders/default3D.glsl");
	Quark::Mesh m_Ball = Quark::Mesh("assets/meshes/arrow.obj");
	Quark::Transform3DComponent m_BallTransform = Quark::Transform3DComponent();

	float m_Speed = 1.0f;
	float m_CoeffX = 0.0f, m_CoeffY = 0.0f;

	bool m_CollidesX = false, m_CollidesY = false;
};

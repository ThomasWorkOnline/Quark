#pragma once

#include <Quark.h>

using namespace Quark;

class Pong : public Application
{
public:
	Pong();

	void OnUpdate(float elapsedTime) override;
	void OnEvent(Event& e) override;

private:
	void FaceOff();
	void ProcessCollision();
	void ProcessCollisionResolution();

	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);

private:
	Scene m_Scene;
	Entity m_Entity;
	PerspectiveCameraController m_Controller;

	Ref<Shader> m_Shader;
	Mesh m_Ball = Mesh("assets/meshes/arrow.obj");
	Transform3DComponent m_BallTransform{};

	float m_Speed = 1.0f;
	float m_CoeffX = 0.0f, m_CoeffY = 0.0f;

	bool m_CollidesX = false, m_CollidesY = false;
};

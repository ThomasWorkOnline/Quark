#pragma once

#include <Quark.h>

using namespace Quark;

class Pong : public Application
{
public:
	Pong(const ApplicationSpecification& spec = {});

	void OnUpdate(Timestep elapsedTime) override;
	void OnRender() override;
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

	StaticMesh m_Ball;
	Scope<Shader> m_Shader;
	Transform3DComponent m_BallTransform;

	Scope<AudioBuffer> m_AudioBuffer;
	Scope<AudioSource> m_AudioSource;
	bool m_AudioPlaying = false;

	float m_Speed = 1.0f;
	float m_CoeffX = 0.0f, m_CoeffY = 0.0f;
	bool m_CollidesX = false, m_CollidesY = false;
};

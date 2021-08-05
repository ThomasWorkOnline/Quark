#pragma once

#include <Quark.h>

class PlayerCameraController : public Quark::OrthographicCameraController
{
public:
	void OnEvent(Quark::Event& e) override;

	void OnUpdate(float elapsedTime) override;
	void OnUpdate(float elapsedTime, Quark::Entity player);

private:
	bool OnMouseScrolled(Quark::MouseScrolledEvent& e);

	Quark::Entity m_Player;
};

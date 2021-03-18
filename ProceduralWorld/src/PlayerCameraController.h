#pragma once

#include <Entropy.h>

class PlayerCameraController : public Entropy::OrthographicCameraController
{
public:
	void OnEvent(Entropy::Event& e) override;

	void OnUpdate(float elapsedTime) override;
	void OnUpdate(float elapsedTime, Entropy::Entity player);

private:
	bool OnMouseScrolled(Entropy::MouseScrolledEvent& e);

	Entropy::Entity m_Player;
};

#pragma once

#include <Quark.h>

using namespace Quark;

class MainLayer : public Layer
{
public:
	MainLayer();

	virtual void OnUpdate(Timestep elapsedTime) override;
	virtual void OnEvent(Event& e) override;

private:
	bool OnWindowResized(WindowResizedEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);

private:
	FontLibrary m_Library;
	Ref<Font> m_Font1;
	Ref<Font> m_Font2;
	Ref<Texture2D> m_Texture;
	Text m_Text;

	glm::vec4 m_Color1 = { 1.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 m_Color2 = { 1.0f, 0.0f, 1.0f, 0.5f };
	glm::mat4 m_Transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f));
	glm::mat4 m_Transform2 = glm::mat4(1.0f);
	glm::mat4 m_Transform3 = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };
	glm::mat4 m_CameraView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
};

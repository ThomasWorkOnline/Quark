#pragma once

#include <Quark.h>

using namespace Quark;

class MainLayer : public Layer
{
public:
	MainLayer();

	virtual void OnUpdate(Timestep elapsedTime) override;
	virtual void OnRender() override;
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
	Text m_Text2;
	TextInput m_Input;

	glm::vec4 m_Color1 = { 1.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 m_Color2 = { 1.0f, 0.0f, 1.0f, 0.5f };
	glm::mat4 m_Transform = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	SceneCamera m_Camera;
};

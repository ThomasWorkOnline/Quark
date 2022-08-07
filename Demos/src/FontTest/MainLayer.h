#pragma once

#include <Quark.h>

using namespace Quark;

class MainLayer : public Layer
{
public:
	MainLayer(Application* app);

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

	Vec4f m_Color1 = { 1.0f, 1.0f, 0.0f, 1.0f };
	Vec4f m_Color2 = { 1.0f, 0.0f, 1.0f, 0.5f };
	Mat4f m_Transform = glm::rotate(Mat4f(1.0f), glm::radians(45.0f), Vec3f(1.0f, 0.0f, 0.0f));

	SceneCamera m_Camera;
};

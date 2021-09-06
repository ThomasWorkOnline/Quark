#pragma once

#include "Quark.h"

using namespace Quark;

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	glm::vec4 Color;
	uint32_t TexIndex;
};

class FontTest : public Application
{
public:
	FontTest()
	{
		m_Font = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 0, 48);

		m_Transform2 = glm::translate(m_Transform2, glm::vec3(-0.4f, -0.3f, 0.0f));
	}

	void OnUpdate(float elapsedTime)
	{
		m_Camera.OnUpdate();

		Renderer::BeginScene(m_Camera.GetProjection(), glm::mat4(1.0f));

		m_Transform = glm::rotate(m_Transform, elapsedTime, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		Renderer::SubmitText(m_Font, m_String, m_Color, m_Transform);

		Renderer::SubmitText(m_Font, m_String2, m_Color2, m_Transform2);
		
		Renderer::EndScene();
	}

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(FontTest::OnWindowResized));
	}

	bool OnWindowResized(WindowResizedEvent& e)
	{
		m_Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());
		return false;
	}

private:
	FontLibrary m_Library;

	Ref<Font> m_Font;
	glm::vec4 m_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 m_Color2 = { 1.0f, 0.0f, 1.0f, 0.5f };
	glm::mat4 m_Transform = glm::mat4(1.0f);
	glm::mat4 m_Transform2 = glm::mat4(1.0f);
	Ref<VertexArray> m_VertexArray;
	Ref<Shader> m_Shader;
	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };

	std::string m_String = "Welcome to Quark!";
	std::string m_String2 = "20000 characters in 1 draw call!";
};

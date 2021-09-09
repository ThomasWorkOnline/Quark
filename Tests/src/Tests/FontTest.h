#pragma once

#include "Quark.h"

using namespace Quark;

class FontTest : public Application
{
public:
	FontTest()
	{
		m_Font = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 0, 128);
		m_Font2 = m_Library.Load("agency-regular", "assets/fonts/ANTQUAI.TTF", 0, 64);

		m_Transform2 = glm::translate(m_Transform2, glm::vec3(-0.4f, -0.3f, 0.0f));

		m_Texture = Texture2D::Create("assets/textures/sprite_sheet.png");
	}

	void OnUpdate(float elapsedTime)
	{
		m_Camera.OnUpdate();

		static float accumTime = elapsedTime;
		accumTime += elapsedTime;

		{
			Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

			Renderer::SubmitSprite(m_Texture, m_Transform3);

			Renderer::EndScene();
		}

		{
			RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);
			Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

			m_Transform = glm::rotate(m_Transform, cosf(accumTime * 2.0f) * 0.001f, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
			Renderer::SubmitText(m_Font2, m_String2, m_Color2, m_Transform2);
			Renderer::SubmitText(m_Font, m_String, m_Color, m_Transform);

			Renderer::EndScene();
			RenderCommand::SetDepthFunction(RenderDepthFunction::Default);
		}
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
	Ref<Font> m_Font2;
	glm::vec4 m_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 m_Color2 = { 1.0f, 0.0f, 1.0f, 0.5f };
	glm::mat4 m_Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f));
	glm::mat4 m_Transform2 = glm::mat4(1.0f);
	glm::mat4 m_Transform3 = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	Ref<VertexArray> m_VertexArray;
	Ref<Shader> m_Shader;
	Ref<Texture2D> m_Texture;

	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };
	glm::mat4 m_CameraView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	std::string m_String = "Quark Engine";
	std::string m_String2 = "Font loading done with Freetype";
};

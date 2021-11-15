#pragma once

#include "Quark.h"

using namespace Quark;

class FontTest : public Application
{
public:
	FontTest()
	{
		TextureArraySpecification spec;
		spec.Width = 48;
		spec.Height = 48;
		spec.Layers = 1;
		spec.DataFormat = TextureDataFormat::RGBA8;
		spec.RenderModes.MagFilteringMode = TextureFilteringMode::Nearest;

		m_Font = m_Library.Load("arial-regular", "assets/fonts/arial.ttf", 0, 128);
		m_Font2 = m_Library.Load("agency-regular", "assets/fonts/ANTQUAI.TTF", 0, 64);

		m_Text = Text("Hello this is a text!", m_Font, m_Color, 1.0f, 1.0f, HorizontalTextAlignment::Left);
		m_Transform2 = glm::translate(m_Transform2, glm::vec3(-0.4f, -0.3f, 0.0f));
		m_Texture = Texture2D::Create("assets/textures/sprite_sheet.png", spec.RenderModes);

		Image image("assets/textures/sprite_sheet.png", true);

		m_TextureArray = Texture2DArray::Create(spec);
		m_TextureArray->SetData(image.GetData(), image.Size(), 0);
	}

	void OnUpdate(float elapsedTime)
	{
		m_Camera.OnUpdate();

		static float accumTime = elapsedTime;
		accumTime += elapsedTime;

		{
			Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

			Renderer::SubmitSprite(m_Texture, m_Transform3);

			RenderCommand::SetDepthFunction(RenderDepthFunction::LessEqual);
			Renderer::SubmitText(m_Text, m_Transform);
			Renderer::SubmitText(m_Font2, "Hi there!", glm::vec4(1.0f), glm::vec2(1.0f));
			RenderCommand::SetDepthFunction(RenderDepthFunction::Default);

			Renderer::EndScene();
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
	Text m_Text;

	glm::vec4 m_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 m_Color2 = { 1.0f, 0.0f, 1.0f, 0.5f };
	glm::mat4 m_Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f));
	glm::mat4 m_Transform2 = glm::mat4(1.0f);
	glm::mat4 m_Transform3 = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	Ref<VertexArray> m_VertexArray;
	Ref<Shader> m_Shader;
	Ref<Texture2D> m_Texture;
	Ref<Texture2DArray> m_TextureArray;

	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };
	glm::mat4 m_CameraView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	std::string m_String = "Quark Engine";
	std::string m_String2 = "Font loading done with Freetype";
};

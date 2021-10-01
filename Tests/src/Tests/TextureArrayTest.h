#pragma once

#include "Quark.h"

using namespace Quark;

class TextureArrayTest : public Application
{
public:
	TextureArrayTest()
	{
		TextureArraySpecification spec;
		spec.Width = 48;
		spec.Height = 48;
		spec.Layers = 1;
		m_TextureArray = Texture2DArray::Create(spec);
		Image image("assets/textures/sprite_sheet.png", true);
		m_TextureArray->SetData(image.GetData(), image.Size(), 0);
	}

	void OnUpdate(float elapsedTime)
	{
		m_Camera.OnUpdate();

		Renderer::BeginScene(m_Camera.GetProjection(), m_CameraView);

		Renderer::EndScene();
	}

	void OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizedEvent>(ATTACH_EVENT_FN(TextureArrayTest::OnWindowResized));
	}

	bool OnWindowResized(WindowResizedEvent& e)
	{
		m_Camera.SetAspectRatio((float)e.GetWidth() / e.GetHeight());
		return false;
	}

private:
	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };
	glm::mat4 m_CameraView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	Ref<Texture2DArray> m_TextureArray;
};

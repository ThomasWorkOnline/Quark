#pragma once

#include "Quark.h"

using namespace Quark;

class TextureArrayTest : public Application
{
public:
	TextureArrayTest();

	void OnUpdate(Timestep elapsedTime) override;
	void OnRender() override;
	void OnEvent(Event& e) override;

private:
	bool OnWindowResized(WindowResizedEvent& e);

private:
	PerspectiveCamera m_Camera = { 16.0f / 9.0f, 70.0f };
	glm::mat4 m_CameraView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 m_Transform = glm::mat4(1.0f);

	Ref<Texture2DArray> m_TextureArray;
	Ref<Shader> m_Shader;

	Ref<VertexArray> m_VertexArray;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;
};

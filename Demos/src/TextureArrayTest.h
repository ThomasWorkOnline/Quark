#pragma once

#include "Quark.h"

using namespace Quark;

class TextureArrayTest : public Application
{
public:
	TextureArrayTest(const ApplicationSpecification& spec = {});

	void OnUpdate(Timestep elapsedTime) override;
	void OnRender() override;
	void OnEvent(Event& e) override;

private:
	bool OnWindowResized(WindowResizedEvent& e);

private:
	SceneCamera m_Camera;
	Mat4f m_CameraTransform = glm::translate(Mat4f(1.0f), Vec3f(0.0f, 0.0f, 1.0f));
	Mat4f m_ModelTransform = Mat4f(1.0f);

	Scope<Texture2DArray> m_TextureArray;
};

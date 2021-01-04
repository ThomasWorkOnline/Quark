///////////////////////////////////////////////////////////////////////////////////////////////
// 1. #include "Entropy.h"                                                                   //
// 2. Create a sub class that inherits from Entropy::Application                             //
// 3. Call Entropy::CreateApplication(width, height, title); returns your class instance     //
// 4. Implement OnUpdate() and OnCreate() pure virtual methods                               //
// You're set!                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Entropy.h"

class SandboxGame : public Entropy::Application
{
public:
	virtual void OnCreate() override;
	virtual void OnUpdate(float elapsedTime) override;

	virtual void OnApplicationEvent(Entropy::Event& e) override;

	bool OnMouseButtonPressed(Entropy::MouseButtonPressedEvent& e);
	bool OnKeyPressed(Entropy::KeyPressedEvent& e);

private:
	Entropy::Ref<Entropy::Texture2D> diffuseMap = Entropy::Texture2D::Create("./assets/textures/container.png");
	Entropy::Ref<Entropy::Texture2D> specularMap = Entropy::Texture2D::Create("./assets/textures/container_specular.png");
	Entropy::Ref<Entropy::Texture2D> normalMap = Entropy::Texture2D::Create("./assets/textures/normal_map.png");
	Entropy::Ref<Entropy::Texture2D> white = Entropy::Texture2D::Create("./assets/textures/white.jpg");
	Entropy::Ref<Entropy::Texture2D> cobblestone = Entropy::Texture2D::Create("./assets/textures/cobblestone.png");

	Entropy::ShaderLibrary m_ShaderLibrary;
	Entropy::Ref<Entropy::Shader> m_DefaultShader;
	Entropy::Ref<Entropy::Shader> m_DebugShader;
	Entropy::Ref<Entropy::Shader> m_SelectedShader;

	Entropy::Mesh m_Plane;
	Entropy::Mesh m_Model;
	Entropy::Mesh m_Sphere;

	glm::mat4 m_PlaneTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 m_SphereTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
	glm::mat4 m_ModelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 m_PointLightPosition = glm::vec3(-0.7f, 2.0f, -5.0f);

	Entropy::CameraController m_CameraController = Entropy::CameraController(GetWindow().GetWidth(), GetWindow().GetHeight());
};

Entropy::Application* Entropy::CreateApplication()
{
	return new SandboxGame();
}

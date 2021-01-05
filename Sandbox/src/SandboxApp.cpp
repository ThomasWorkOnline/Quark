#include "SandboxApp.h"

void SandboxGame::OnCreate()
{
	m_CameraController.GetCamera().SetPosition({ 0.0f, 1.0f, -8.0f });

	m_DefaultShader = m_ShaderLibrary.Load("./assets/shaders/default.glsl");
	m_DebugShader = m_ShaderLibrary.Load("./assets/shaders/unlitPosGradient.glsl");
	m_SelectedShader = m_DefaultShader;

	m_SelectedShader->Attach();
	m_SelectedShader->SetInt("u_Material.diffuse", 0);
	m_SelectedShader->SetInt("u_Material.specular", 1);
	m_SelectedShader->SetInt("u_Material.normalMap", 2);

	//m_Model.GenerateUnitCube();
	m_Model.LoadOBJFromFile("./assets/models/monkey.obj");
	m_Sphere.LoadOBJFromFile("./assets/models/sphere.obj");

	m_Plane.GenerateTerrain(10, 0);

	float lightPower = 5.0f;
	glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
	m_SelectedShader->SetFloat3("u_Light.position", m_PointLightPosition);
	m_SelectedShader->SetFloat3("u_Light.ambient", lightColor * 0.005f * lightPower);
	m_SelectedShader->SetFloat3("u_Light.diffuse", lightColor * 0.2f * lightPower);
	m_SelectedShader->SetFloat3("u_Light.specular", lightColor * lightPower);
	// Light will cover a distance of 50 meters
	m_SelectedShader->SetFloat("u_Light.constant", 1.0f);
	m_SelectedShader->SetFloat("u_Light.linear", 0.09f);
	m_SelectedShader->SetFloat("u_Light.quadratic", 0.032f);

	m_SelectedShader->SetFloat("u_Material.shininess", 512.0f);
}

void SandboxGame::OnUpdate(float elapsedTime)
{
	static float accumulatedTime = 0.0f;
	accumulatedTime += elapsedTime;

	if (accumulatedTime > 1.0f)
	{
		std::cout << std::fixed << 1.0f / elapsedTime << " fps | frametime: " << elapsedTime * 1000 << "ms\n";
		accumulatedTime = 0.0f;

		/*std::cout << "Position: " << m_CameraController.GetCamera().GetPosition() << std::endl;
		std::cout << "Velocity: " << m_CameraController.GetCamera().GetVelocity() << std::endl;
		std::cout << "Orientation: " << m_CameraController.GetCamera().GetOrientation() << std::endl;*/
	}

	const float speed = 10.0f;
	if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::RightShift))
	{
		if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Up))
		{
			m_PointLightPosition.y += elapsedTime * speed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Down))
		{
			m_PointLightPosition.y -= elapsedTime * speed;
		}
	}
	else
	{
		if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Up))
		{
			m_PointLightPosition.z += elapsedTime * speed;
		}

		if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Down))
		{
			m_PointLightPosition.z -= elapsedTime * speed;
		}
	}

	if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Right))
	{
		m_PointLightPosition.x += elapsedTime * speed;
	}

	if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Left))
	{
		m_PointLightPosition.x -= elapsedTime * speed;
	}

	m_CameraController.OnUpdate(elapsedTime);

	m_PortalCameraController.GetCamera().SetPosition({ 0.0f, 1.0f, -4.0f });
	m_PortalCameraController.GetCamera().SetOrientation(0.0f, { 1.0f, 0.0f, 0.0f });
	m_PortalCameraController.GetCamera().SetFov(45.0f);
	m_PortalCameraController.OnUpdate(elapsedTime);

	m_SelectedShader->SetFloat3("u_Light.position", m_PointLightPosition);

	//m_ModelTransform = glm::translate(m_ModelTransform, elapsedTime * glm::vec3(0.0f, 0.0f, 0.0f));
	m_ModelTransform = glm::rotate(m_ModelTransform, elapsedTime, glm::vec3(0.0f, 1.0f, 0.0f));

	{
		Entropy::Renderer::BeginScene(m_PortalCameraController.GetCamera());

		m_Framebuffer->Attach();

		// Drawing Suzanne to the framebuffer
		Entropy::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Entropy::RenderCommand::Clear();
		white->Attach(0);
		white->Attach(1);
		Entropy::Renderer::Submit(m_SelectedShader, m_Model.GetVertexArray(), m_ModelTransform);

		m_Framebuffer->Detach();

		Entropy::Renderer::EndScene();
	}
		
	Entropy::RenderCommand::SetViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

	{
		Entropy::Renderer::BeginScene(m_CameraController.GetCamera());

		white->Attach(0);
		white->Attach(1);
		Entropy::Renderer::Submit(m_SelectedShader, m_Model.GetVertexArray(), m_ModelTransform);

		m_Framebuffer->AttachColorAttachment(0);
		//diffuseMap->Attach(0);
		specularMap->Attach(1);
		Entropy::Renderer::Submit(m_SelectedShader, m_Plane.GetVertexArray(), m_PlaneTransform);

		white->Attach(0);
		white->Attach(1);
		Entropy::Renderer::Submit(m_SelectedShader, m_Sphere.GetVertexArray(), glm::translate(glm::mat4(1.0f), m_PointLightPosition) * m_SphereTransform);

		Entropy::Renderer::EndScene();
	}
}

void SandboxGame::OnApplicationEvent(Entropy::Event& e)
{
	Entropy::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Entropy::MouseButtonPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnMouseButtonPressed));
	dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnKeyPressed));

	m_CameraController.OnEvent(e);
}

bool SandboxGame::OnMouseButtonPressed(Entropy::MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
	case Entropy::MouseCode::Button0:
	{
		// Focus enabled
		GetWindow().Select();
		break;
	}
	}
	return false;
}

bool SandboxGame::OnKeyPressed(Entropy::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case Entropy::KeyCode::F1:
	{
		// Toggle debug shader
		static bool debugShaderEnabled = false;
		debugShaderEnabled = !debugShaderEnabled;
		m_SelectedShader = debugShaderEnabled ? m_DebugShader : m_DefaultShader;

		std::stringstream ss;
		ss << "Shader used: ";
		ss << m_SelectedShader->GetName();
		NT_TRACE(ss.str());
		break;
	}
	case Entropy::KeyCode::F3:
	{
		// Toggle VSync
		auto& window = GetWindow();
		window.SetVSync(!window.IsVSync());

		std::stringstream ss;
		ss << "VSync is now ";
		ss << (window.IsVSync() ? "enabled" : "disabled");
		NT_TRACE(ss.str());
		break;
	}
	case Entropy::KeyCode::F11:
	{
		auto& window = GetWindow();
		window.EnableFullScreen();
		break;
	}
	case Entropy::KeyCode::Escape:
	{
		// Focus lost
		GetWindow().Deselect();
		break;
	}
	}
	return false;
}

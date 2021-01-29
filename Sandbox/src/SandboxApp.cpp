///////////////////////////////////////////////////////////////////////////////////////////////
// 1. #include "Entropy.h"                                                                   //
// 2. Create a sub class that inherits from Entropy::Application                             //
// 3. Call Entropy::CreateApplication() returns your class instance                          //
// 4. Implement OnUpdate() and OnCreate() pure virtual methods                               //
// You're set!                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Entropy.h"

class SandboxGame : public Entropy::Application
{
public:
	void OnCreate() override
	{
		auto& window = GetWindow();

		{
			m_CameraEntity.AddComponent<Entropy::TransformComponent>().Position = { 0.0f, 1.0f, -8.0f };
			m_CameraEntity.AddComponent<Entropy::PhysicsComponent>();
			m_CameraEntity.AddComponent<Entropy::CameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 45.0f);
		}

		{
			m_PortalCamera.AddComponent<Entropy::TransformComponent>().Position = { 0.0f, 0.0f, -6.0f };
			m_PortalCamera.AddComponent<Entropy::PhysicsComponent>();
			m_PortalCamera.AddComponent<Entropy::CameraComponent>(1.0f, 45.0f);
		}

		{
			m_PlaneEntity.AddComponent<Entropy::TransformComponent>();
			m_PlaneEntity.AddComponent<Entropy::MeshComponent>().Mesh.GenerateTerrain(10, 0);
		}

		{
			m_ModelEntity.AddComponent<Entropy::TransformComponent>().Position.y = 1.5f;
			m_ModelEntity.AddComponent<Entropy::MeshComponent>("./assets/models/monkey.obj");
		}

		{
			m_SphereEntity.AddComponent<Entropy::TransformComponent>();
			m_SphereEntity.AddComponent<Entropy::MeshComponent>("./assets/models/sphere.obj");
		}

		m_DefaultShader = m_ShaderLibrary.Load("./assets/shaders/default.glsl");
		m_DefaultFlatShader = m_ShaderLibrary.Load("./assets/shaders/defaultFlat.glsl");
		m_DebugNormalShader = m_ShaderLibrary.Load("./assets/shaders/debugNormal.glsl");
		m_SelectedShader = m_DefaultShader;
	}

	void OnUpdate(float elapsedTime) override
	{
		m_Scene.OnUpdate(elapsedTime);
		m_CameraController.OnUpdate(elapsedTime);

		static float accumulatedTime = 0.0f;
		accumulatedTime += elapsedTime;

		if (accumulatedTime > 1.0f)
		{
			std::cout << std::fixed << 1.0f / elapsedTime << " fps | frametime: " << elapsedTime * 1000 << "ms\n";
			accumulatedTime = 0.0f;

			auto& window = GetWindow();
			std::cout << "Window resolution: " << window.GetWidth() << "x" << window.GetHeight() << std::endl << std::endl;

			auto& transform = m_CameraEntity.GetComponent<Entropy::TransformComponent>();
			std::cout << "Position: " << transform.Position << std::endl;
			std::cout << "Orientation: " << transform.Orientation << std::endl;
			std::cout << "Transform: " << transform.Scale << std::endl << std::endl;

			std::cout << "Framebuffer size: " << m_Spec.Width << "x" << m_Spec.Height << std::endl << std::endl;
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

		if (Entropy::Input::IsMouseButtonPressed(Entropy::MouseCode::Button3))
		{
			m_NormalLength -= elapsedTime;

			if (m_NormalLength < 0.001f)
				m_NormalLength = 0.001f;
		}

		if (Entropy::Input::IsMouseButtonPressed(Entropy::MouseCode::Button4))
		{
			m_NormalLength += elapsedTime;

			if (m_NormalLength > 1.0f)
				m_NormalLength = 1.0f;
		}

		m_SelectedShader->Attach();
		m_SelectedShader->SetFloat3("u_Light.position", m_PointLightPosition);
		m_SelectedShader->SetInt("u_Material.diffuse", 0);
		m_SelectedShader->SetInt("u_Material.specular", 1);
		m_SelectedShader->SetInt("u_Material.normalMap", 2);
		constexpr float lightPower = 5.0f;
		constexpr glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
		m_SelectedShader->SetFloat3("u_Light.position", m_PointLightPosition);
		m_SelectedShader->SetFloat3("u_Light.ambient", lightColor * 0.005f * lightPower);
		m_SelectedShader->SetFloat3("u_Light.diffuse", lightColor * 0.2f * lightPower);
		m_SelectedShader->SetFloat3("u_Light.specular", lightColor * lightPower);
		// Light will cover a distance of 50 meters
		m_SelectedShader->SetFloat("u_Light.constant", 1.0f);
		m_SelectedShader->SetFloat("u_Light.linear", 0.09f);
		m_SelectedShader->SetFloat("u_Light.quadratic", 0.032f);
		m_SelectedShader->SetFloat("u_Material.shininess", 512.0f);
		m_SelectedShader->SetFloat("u_NormalLength", m_NormalLength);

		m_ModelEntity.GetComponent<Entropy::TransformComponent>().Rotate(elapsedTime, glm::vec3(0.0f, 1.0f, 0.0f));
		auto& transform = m_SphereEntity.GetComponent<Entropy::TransformComponent>();
		transform.Scale = { 0.01f, 0.01f, 0.01f };
		transform.Position = m_PointLightPosition;

		{
			Entropy::Renderer::BeginScene(m_PortalCamera);

			m_Framebuffer->Attach();

			// Drawing Suzanne to the framebuffer
			Entropy::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			Entropy::RenderCommand::Clear();
			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity);

			m_Framebuffer->Detach();
			Entropy::RenderCommand::SetViewport(0, 0, GetWindow().GetWidth(), GetWindow().GetHeight());

			Entropy::Renderer::EndScene();
		}

		{
			Entropy::Renderer::BeginScene(m_CameraEntity);

			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity);

			m_Framebuffer->AttachColorAttachment(0);
			m_Framebuffer->AttachColorAttachment(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_PlaneEntity);

			white->Attach(0);
			white->Attach(1);
			Entropy::Renderer::Submit(m_SelectedShader, m_SphereEntity);

			Entropy::Renderer::EndScene();
		}
	}

	void OnApplicationEvent(Entropy::Event& e) override
	{
		Entropy::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Entropy::MouseButtonPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnMouseButtonPressed));
		dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnKeyPressed));
		dispatcher.Dispatch<Entropy::WindowResizeEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnWindowResized));

		m_CameraController.OnEvent(e);
	}

	bool OnWindowResized(Entropy::WindowResizeEvent& e)
	{
		return false;
	}

	bool OnMouseButtonPressed(Entropy::MouseButtonPressedEvent& e)
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

	bool OnKeyPressed(Entropy::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Entropy::KeyCode::F1:
		{
			// Toggle debug shader
			static uint32_t shaderIndex = 0;
			shaderIndex++;

			if (shaderIndex > 2)
				shaderIndex = 0;

			switch (shaderIndex)
			{
			case 0:
				m_SelectedShader = m_DefaultShader;
				break;
			case 1:
				m_SelectedShader = m_DefaultFlatShader;
				break;
			case 2:
				m_SelectedShader = m_DebugNormalShader;
				break;
			}

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
			GetWindow().EnableFullScreen();
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

private:
	Entropy::Scene m_Scene = Entropy::Scene();

	Entropy::Ref<Entropy::Texture2D> white = Entropy::Texture2D::Create("./assets/textures/white.jpg");
	Entropy::Ref<Entropy::Texture2D> diffuseMap = Entropy::Texture2D::Create("./assets/textures/container.png");
	Entropy::Ref<Entropy::Texture2D> specularMap = Entropy::Texture2D::Create("./assets/textures/container_specular.png");
	//Entropy::Ref<Entropy::Texture2D> normalMap = Entropy::Texture2D::Create("./assets/textures/normal_map.png");
	//Entropy::Ref<Entropy::Texture2D> cobblestone = Entropy::Texture2D::Create("./assets/textures/cobblestone.png");

	Entropy::ShaderLibrary m_ShaderLibrary;
	Entropy::Ref<Entropy::Shader> m_DefaultShader;
	Entropy::Ref<Entropy::Shader> m_DefaultFlatShader;
	Entropy::Ref<Entropy::Shader> m_DebugNormalShader;
	Entropy::Ref<Entropy::Shader> m_SelectedShader;
	float m_NormalLength = 0.1f;

	Entropy::Entity m_PlaneEntity = m_Scene.CreateEntity();
	Entropy::Entity m_ModelEntity = m_Scene.CreateEntity();
	Entropy::Entity m_SphereEntity = m_Scene.CreateEntity();
	Entropy::Entity m_CameraEntity = m_Scene.CreateEntity();
	Entropy::Entity m_PortalCamera = m_Scene.CreateEntity();

	Entropy::CameraController m_CameraController = Entropy::CameraController(m_CameraEntity);
	glm::vec3 m_PointLightPosition = glm::vec3(-0.7f, 2.0f, -5.0f);

	Entropy::FramebufferSpecification m_Spec = { 1024, 1024, { { Entropy::FramebufferTextureFormat::RGBA8, Entropy::FramebufferFilteringFormat::Linear }, { Entropy::FramebufferTextureFormat::Depth24Stencil8 } } };
	Entropy::Ref<Entropy::Framebuffer> m_Framebuffer = Entropy::Framebuffer::Create(m_Spec);
};

Entropy::Application* Entropy::CreateApplication()
{
	return new SandboxGame();
}

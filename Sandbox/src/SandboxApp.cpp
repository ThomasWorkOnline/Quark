///////////////////////////////////////////////////////////////////////////////////////////////
// 1. #include <Entropy.h>                                                                   //
// 2. Create a sub class that inherits from Entropy::Application                             //
// 3. Call Entropy::CreateApplication() returns your class instance                          //
// 4. Implement OnUpdate() and OnCreate() pure virtual methods                               //
// You're set!                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////

#include <Entropy.h>

class SandboxGame : public Entropy::Application
{
public:
	void OnCreate() override
	{
		//NT_TIME_SCOPE_DEBUG(SandboxApp::OnCreate);

		auto& window = GetWindow();
		Entropy::BufferLayout layout = {
				{ Entropy::ShaderDataType::Float3, "a_Position" },
				{ Entropy::ShaderDataType::Float2, "a_TexCoord" },
				{ Entropy::ShaderDataType::Float3, "a_Normal" }
		};

		{
			m_CameraEntity.AddComponent<Entropy::Transform3DComponent>().Position = { 0.0f, 0.0f, 0.0f };
			m_CameraEntity.AddComponent<Entropy::PhysicsComponent>();
			m_CameraEntity.AddComponent<Entropy::PerspectiveCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 50.0f);
		}

		{
			m_OrthoCameraEntity.AddComponent<Entropy::Transform3DComponent>();
			m_OrthoCameraEntity.AddComponent<Entropy::PhysicsComponent>();
			m_OrthoCameraEntity.AddComponent<Entropy::OrthographicCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 8.0f);
		}

		{
			m_PortalCameraEntity.AddComponent<Entropy::Transform3DComponent>().Position = { 0.0f, 0.0f, -6.0f };
			m_PortalCameraEntity.AddComponent<Entropy::PhysicsComponent>();
			m_PortalCameraEntity.AddComponent<Entropy::PerspectiveCameraComponent>(1.0f, 45.0f);
		}

		{
			auto& transform = m_PlaneEntity.AddComponent<Entropy::Transform3DComponent>();
			transform.Position.y = -2.0f;
			m_PlaneEntity.AddComponent<Entropy::MeshComponent>().Mesh.GenerateTerrain(layout, 10, 0);
		}

		{
			auto& transform = m_ModelEntity.AddComponent<Entropy::Transform3DComponent>();
			//transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position.y += 1.3f;
			m_ModelEntity.AddComponent<Entropy::MeshComponent>(layout, "./assets/models/monkey.obj");
		}

		{
			auto& transform = m_LightEntity.AddComponent<Entropy::Transform3DComponent>();
			transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position = { -0.7, 1.8f, -4.3f };
			m_LightEntity.AddComponent<Entropy::MeshComponent>(layout, "./assets/models/sphere.obj");
		}

		m_ShaderLibrary.Load("default", "./assets/shaders/default.glsl");
		m_ShaderLibrary.Load("defaultMultisampled", "./assets/shaders/defaultMultisampled.glsl");
		m_ShaderLibrary.Load("defaultFlat", "./assets/shaders/defaultFlat.glsl");
		m_ShaderLibrary.Load("debugNormal", "./assets/shaders/debugNormal.glsl");
		m_ShaderLibrary.Load("debugPosition", "./assets/shaders/debugPosition.glsl");
		m_ShaderLibrary.Load("skybox", "./assets/shaders/skybox.glsl");
		m_SelectedShader = m_ShaderLibrary.Get("default");
	}

	void OnUpdate(float elapsedTime) override
	{
		// Update scene before updating the camera controller
		// The camera controller will use the updated components
		// The other way around would result in 1 frame offset from the actual scene data
		m_Scene.OnUpdate(elapsedTime);
		m_CoinAnimator.OnUpdate(elapsedTime);

		if (m_Perspective)
			m_CameraController.OnUpdate(elapsedTime);
		else
			m_OrthoCameraController.OnUpdate(elapsedTime);

#		if defined(NT_DEBUG) && 0
		static float accumulatedTime = 0.0f;
		accumulatedTime += elapsedTime;

		if (accumulatedTime > 1.0f)
		{
			std::cout << std::fixed << 1.0f / elapsedTime << " fps | frametime: " << elapsedTime * 1000 << "ms\n";
			accumulatedTime = 0.0f;
		}
#		endif

		{
			auto& position = m_LightEntity.GetComponent<Entropy::Transform3DComponent>().Position;
			static const float speed = 10.0f;
			if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::RightShift))
			{
				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Up))
				{
					position.y += elapsedTime * speed;
				}

				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Down))
				{
					position.y -= elapsedTime * speed;
				}
			}
			else
			{
				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Up))
				{
					position.z += elapsedTime * speed;
				}

				if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Down))
				{
					position.z -= elapsedTime * speed;
				}
			}

			if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Right))
			{
				position.x += elapsedTime * speed;
			}

			if (Entropy::Input::IsKeyPressed(Entropy::KeyCode::Left))
			{
				position.x -= elapsedTime * speed;
			}

			m_SelectedShader->Attach();
			m_SelectedShader->SetFloat3("u_Light.position", position);

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
		}

		{
			// TODO: render pipeline and proper light object
			m_SelectedShader->Attach();
			m_SelectedShader->SetInt("u_Material.diffuse", 0);
			m_SelectedShader->SetInt("u_Material.specular", 1);
			m_SelectedShader->SetInt("u_Material.normalMap", 2);
			m_SelectedShader->SetInt("u_Cubemap", 3);
			m_SelectedShader->SetFloat("u_Material.shininess", 512.0f);
			m_SelectedShader->SetFloat("u_Material.metalness", 1.0f);

			constexpr float lightPower = 4.0f;
			constexpr glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f) * lightPower;
			m_SelectedShader->SetFloat3("u_Light.ambient", lightColor * 0.005f);
			m_SelectedShader->SetFloat3("u_Light.diffuse", lightColor * 0.2f);
			m_SelectedShader->SetFloat3("u_Light.specular", lightColor);
			// Light will cover a distance of 50 meters
			m_SelectedShader->SetFloat("u_Light.constant", 1.0f);
			m_SelectedShader->SetFloat("u_Light.linear", 0.09f);
			m_SelectedShader->SetFloat("u_Light.quadratic", 0.032f);

			m_SelectedShader->SetFloat("u_NormalLength", m_NormalLength);

			m_Skybox.GetCubeMap()->Attach(3);

			auto shader = m_ShaderLibrary.Get("skybox");
			shader->Attach();
			shader->SetInt("u_Cubemap", 0);
		}

		m_ModelEntity.GetComponent<Entropy::Transform3DComponent>().Rotate(elapsedTime * 8.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		/*{
			Entropy::Renderer::BeginScene(m_PortalCameraEntity);

			m_Framebuffer->Attach();
			Entropy::RenderCommand::SetViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());

			// Drawing Suzanne to the framebuffer
			Entropy::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			Entropy::RenderCommand::Clear();

			Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity.GetComponent<Entropy::MeshComponent>().Mesh.GetVertexArray(), m_ModelEntity.GetComponent<Entropy::TransformComponent>());

			m_Framebuffer->Detach();
			Entropy::RenderCommand::SetViewport(0, 0, GetWindow().GetWidth(), GetWindow().GetHeight());

			Entropy::Renderer::EndScene();
		}*/
		
		if (m_Perspective)
		{
			{
				auto& cameraTransform = m_CameraEntity.GetComponent<Entropy::Transform3DComponent>();
				Entropy::Renderer::BeginScene(m_CameraEntity.GetComponent<Entropy::PerspectiveCameraComponent>().Camera.GetMatrix(), cameraTransform);

				Entropy::Renderer::SubmitSprite(m_CoinAnimator.GetTexture(), glm::mat4(1.0f));

				Entropy::Renderer::Submit(m_SelectedShader, m_ModelEntity.GetComponent<Entropy::MeshComponent>().Mesh.GetVertexArray(), m_ModelEntity.GetComponent<Entropy::Transform3DComponent>());
				Entropy::Renderer::Submit(m_SelectedShader, m_LightEntity.GetComponent<Entropy::MeshComponent>().Mesh.GetVertexArray(), m_LightEntity.GetComponent<Entropy::Transform3DComponent>());
				Entropy::Renderer::Submit(m_SelectedShader, m_Framebuffer, m_PlaneEntity.GetComponent<Entropy::MeshComponent>().Mesh.GetVertexArray(), m_PlaneEntity.GetComponent<Entropy::Transform3DComponent>());

				Entropy::Renderer::EndScene();
			}

			{
				glm::mat4 view = (glm::mat3)m_CameraEntity.GetComponent<Entropy::Transform3DComponent>();
				Entropy::Renderer::BeginScene(m_CameraEntity.GetComponent<Entropy::PerspectiveCameraComponent>().Camera.GetMatrix(), view);

				Entropy::RenderCommand::SetCullFace(Entropy::RenderCullFace::Back);
				Entropy::RenderCommand::SetDepthFunction(Entropy::RenderDepthFunction::LessEqual);

				m_Skybox.GetCubeMap()->Attach(0);
				Entropy::Renderer::Submit(m_ShaderLibrary.Get("skybox"), m_Skybox.GetVertexArray());

				Entropy::RenderCommand::SetCullFace(Entropy::RenderCullFace::Front);
				Entropy::RenderCommand::SetDepthFunction(Entropy::RenderDepthFunction::Less);

				Entropy::Renderer::EndScene();
			}
		}
		else
		{
			auto& orthoCameraTransform = m_OrthoCameraEntity.GetComponent<Entropy::Transform3DComponent>();
			Entropy::Renderer::BeginScene(m_OrthoCameraEntity.GetComponent<Entropy::OrthographicCameraComponent>().Camera.GetMatrix(), orthoCameraTransform);

			static Entropy::Transform3DComponent transform;
			transform.Position.x = Entropy::Input::GetMouseX() * 0.01f;
			transform.Position.y = Entropy::Input::GetMouseY() * 0.01f;

			transform.Scale.x = (float)m_Maxwell->GetWidth() / (float)m_Maxwell->GetHeight();
			Entropy::Renderer::SubmitSprite(m_Maxwell, transform);

			constexpr int32_t halfScale = 20;
			static Entropy::Transform3DComponent trans;
			for (int y = -halfScale; y < halfScale; y++)
			{
				for (int x = -halfScale; x < halfScale; x++)
				{	
					trans.Position = { y, x, 0.0f };
					Entropy::Renderer::SubmitSprite(m_Cobblestone, trans);
				}
			}

			Entropy::Renderer::EndScene();
		}
	}

	void OnDestroy() override
	{
		NT_TIME_SCOPE_DEBUG(SandboxApp::OnDestroy);

		m_Scene.DeleteEntity(m_PlaneEntity);
		m_Scene.DeleteEntity(m_ModelEntity);
		m_Scene.DeleteEntity(m_LightEntity);
		m_Scene.DeleteEntity(m_CameraEntity);
		m_Scene.DeleteEntity(m_OrthoCameraEntity);
		m_Scene.DeleteEntity(m_PortalCameraEntity);
	}

	void OnEvent(Entropy::Event& e) override
	{
		//NT_TIME_SCOPE_DEBUG(SandboxApp::OnEvent);

		Entropy::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Entropy::MouseButtonPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnMouseButtonPressed));
		dispatcher.Dispatch<Entropy::KeyPressedEvent>(NT_ATTACH_EVENT_FN(SandboxGame::OnKeyPressed));

		if (m_Perspective)
			m_CameraController.OnEvent(e);
		else
			m_OrthoCameraController.OnEvent(e);
	}

	bool OnMouseButtonPressed(Entropy::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Entropy::MouseCode::ButtonLeft:
		{
			// Focus enabled
			if (GetWindow().IsSelected())
				Entropy::AudioEngine::PlaySound("assets/audio/bloop.wav");

			GetWindow().Select();
			break;
		}
		case Entropy::MouseCode::ButtonRight:
		{
			// Focus enabled
			if (GetWindow().IsSelected())
				Entropy::AudioEngine::PlaySound("assets/audio/bloop2.wav");
			break;
		}
		}
		return false;
	}

	bool OnKeyPressed(Entropy::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Entropy::Key::F1:
		{
			// Toggle debug shader
			static uint32_t shaderIndex = 0;
			shaderIndex++;

			if (shaderIndex >= 4)
				shaderIndex = 0;

			switch (shaderIndex)
			{
			case 0:
				m_SelectedShader = m_ShaderLibrary.Get("default");
				break;
			case 1:
				m_SelectedShader = m_ShaderLibrary.Get("defaultFlat");
				break;
			case 2:
				m_SelectedShader = m_ShaderLibrary.Get("debugNormal");
				break;
			case 3:
				m_SelectedShader = m_ShaderLibrary.Get("debugPosition");
			}

			std::stringstream ss;
			ss << "Shader used: ";
			ss << m_SelectedShader->GetName();
			NT_TRACE(ss.str());
			break;
		}
		case Entropy::Key::F2:
		{
			m_Perspective = !m_Perspective;
			break;
		}
		case Entropy::Key::F3:
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
		/*
		case Entropy::Key::F4:
		{
			static uint32_t environmentIndex = 0;
			environmentIndex++;

			if (environmentIndex >= 5)
				environmentIndex = 0;

			switch (environmentIndex)
			{
			case 0:
				m_Skybox.SetEnvironment("./assets/environments/Lycksele3");
				break;
			case 1:
				m_Skybox.SetEnvironment("./assets/environments/FishermansBastion");
				break;
			case 2:
				m_Skybox.SetEnvironment("./assets/environments/GoldenGateBridge");
				break;
			case 3:
				m_Skybox.SetEnvironment("./assets/environments/GoldenGateBridge2");
				break;
			case 4:
				m_Skybox.SetEnvironment("./assets/environments/Storforsen4");
				break;
			}

			NT_TRACE("Switched environment");

			break;
		}
		*/
		case Entropy::Key::F11:
		{
			auto& window = GetWindow();
			if (window.IsFullscreen())
			{
				window.DisableFullScreen();
				NT_TRACE("Disabled fullscreen");
			}
			else
			{
				window.EnableFullScreen();
				NT_TRACE("Enabled fullscreen");
			}
			break;
		}
		case Entropy::Key::Escape:
		{
			// Focus lost
			GetWindow().Deselect();
			break;
		}
		}

		return false;
	}

private:
	Entropy::Scene m_Scene;

	Entropy::Entity m_PlaneEntity = m_Scene.CreateEntity();
	Entropy::Entity m_ModelEntity = m_Scene.CreateEntity();
	Entropy::Entity m_LightEntity = m_Scene.CreateEntity();
	Entropy::Entity m_CameraEntity = m_Scene.CreateEntity();
	Entropy::Entity m_OrthoCameraEntity = m_Scene.CreateEntity();
	Entropy::Entity m_PortalCameraEntity = m_Scene.CreateEntity();

	Entropy::Ref<Entropy::Texture2D> m_DiffuseMap  = Entropy::Texture2D::Create("./assets/textures/container.png");
	Entropy::Ref<Entropy::Texture2D> m_SpecularMap = Entropy::Texture2D::Create("./assets/textures/container_specular.png");
	Entropy::Ref<Entropy::Texture2D> m_NormalMap   = Entropy::Texture2D::Create("./assets/textures/normal_map.png");
	Entropy::Ref<Entropy::Texture2D> m_Cobblestone = Entropy::Texture2D::Create("./assets/textures/cobblestone.png");
	Entropy::Ref<Entropy::Texture2D> m_CoinSheet   = Entropy::Texture2D::Create("./assets/textures/coin.png");
	Entropy::Ref<Entropy::Texture2D> m_Maxwell     = Entropy::Texture2D::Create("./assets/textures/Maxwell.png");

	Entropy::SpriteAnimator m_CoinAnimator = { {
		{ m_CoinSheet, { 0, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 1, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 2, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 3, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 4, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 5, 0 }, { 16, 16 } } }, 8.0f
	};

	Entropy::ShaderLibrary m_ShaderLibrary;
	Entropy::Ref<Entropy::Shader> m_SelectedShader;
	float m_NormalLength = 0.1f;

	Entropy::UIElement m_Element;

	Entropy::Environment m_Skybox = { "./assets/environments/Lycksele3" };

	Entropy::PerspectiveCameraController m_CameraController = { m_CameraEntity };
	Entropy::OrthographicCameraController m_OrthoCameraController = { m_OrthoCameraEntity };

	// TODO: fix multisampling
	Entropy::FramebufferSpecification m_FramebufferSpec = { 1024, 1024, 1, {
		{ Entropy::TextureDataFormat::RGB8, Entropy::TextureFilteringFormat::Linear, Entropy::TextureTilingFormat::Repeat },
		{ Entropy::TextureDataFormat::Depth24Stencil8 } } };
	Entropy::Ref<Entropy::Framebuffer> m_Framebuffer = Entropy::Framebuffer::Create(m_FramebufferSpec);

	bool m_Perspective = false;
};

int main()
{
	auto app = new SandboxGame();
	app->Run();
	delete app;

	return 0;
}

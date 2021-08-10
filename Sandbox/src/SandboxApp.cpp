///////////////////////////////////////////////////////////////////////////////////////////////
// 1. #include <Quark.h>                                                                     //
// 2. Create a sub class that inherits from Quark::Application                               //
// 3. Call Quark::CreateApplication() returns your class instance                            //
// 4. Implement OnUpdate() and OnCreate() pure virtual methods                               //
// You're set!                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////

#include <Quark.h>

class SandboxGame : public Quark::Application
{
public:
	void OnCreate() override
	{
		QK_TIME_SCOPE_DEBUG(SandboxApp::OnCreate);

		auto& window = GetWindow();
		Quark::BufferLayout layout = {
				{ Quark::ShaderDataType::Float3, "a_Position" },
				{ Quark::ShaderDataType::Float2, "a_TexCoord" },
				{ Quark::ShaderDataType::Float3, "a_Normal" }
		};

		{
			m_CameraEntity.AddComponent<Quark::Transform3DComponent>().Position = { 0.0f, 0.0f, 0.0f };
			m_CameraEntity.AddComponent<Quark::PhysicsComponent>().Friction = 4.f;
			m_CameraEntity.AddComponent<Quark::PerspectiveCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 50.0f);
		}

		{
			m_OrthoCameraEntity.AddComponent<Quark::Transform3DComponent>();
			m_OrthoCameraEntity.AddComponent<Quark::PhysicsComponent>().Friction = 4.f;
			m_OrthoCameraEntity.AddComponent<Quark::OrthographicCameraComponent>((float)window.GetWidth() / (float)window.GetHeight(), 8.0f);
		}

		{
			m_PortalCameraEntity.AddComponent<Quark::Transform3DComponent>().Position = { 0.0f, 0.0f, -6.0f };
			m_PortalCameraEntity.AddComponent<Quark::PhysicsComponent>();
			m_PortalCameraEntity.AddComponent<Quark::PerspectiveCameraComponent>(1.0f, 45.0f);
		}

		{
			auto& transform = m_PlaneEntity.AddComponent<Quark::Transform3DComponent>();
			transform.Position.y = -2.0f;
			m_PlaneEntity.AddComponent<Quark::MeshComponent>().Mesh.GenerateTerrain(layout, 100, 0);
		}

		{
			auto& transform = m_ModelEntity.AddComponent<Quark::Transform3DComponent>();
			//transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position.y += 1.3f;
			m_ModelEntity.AddComponent<Quark::MeshComponent>(layout, "./assets/models/monkey.obj");
		}

		{
			auto& transform = m_LightEntity.AddComponent<Quark::Transform3DComponent>();
			transform.Scale = { 0.01f, 0.01f, 0.01f };
			transform.Position = { -0.7, 1.8f, -4.3f };
			m_LightEntity.AddComponent<Quark::MeshComponent>(layout, "./assets/models/sphere.obj");
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
#		if defined(QK_DEBUG) && 0
		static float accumulatedTime = 0.0f;
		accumulatedTime += elapsedTime;

		if (accumulatedTime > 1.0f)
		{
			std::cout << std::fixed << 1.0f / elapsedTime << " fps | frametime: " << elapsedTime * 1000 << "ms\n";
			accumulatedTime = 0.0f;
		}
#		endif

		{
			auto& position = m_LightEntity.GetComponent<Quark::Transform3DComponent>().Position;
			static constexpr float speed = 10.0f;
			if (Quark::Input::IsKeyPressed(Quark::KeyCode::RightShift))
			{
				if (Quark::Input::IsKeyPressed(Quark::KeyCode::Up))
				{
					position.y += elapsedTime * speed;
				}

				if (Quark::Input::IsKeyPressed(Quark::KeyCode::Down))
				{
					position.y -= elapsedTime * speed;
				}
			}
			else
			{
				if (Quark::Input::IsKeyPressed(Quark::KeyCode::Up))
				{
					position.z += elapsedTime * speed;
				}

				if (Quark::Input::IsKeyPressed(Quark::KeyCode::Down))
				{
					position.z -= elapsedTime * speed;
				}
			}

			if (Quark::Input::IsKeyPressed(Quark::KeyCode::Right))
			{
				position.x += elapsedTime * speed;
			}

			if (Quark::Input::IsKeyPressed(Quark::KeyCode::Left))
			{
				position.x -= elapsedTime * speed;
			}

			m_SelectedShader->Attach();
			m_SelectedShader->SetFloat3("u_Light.position", position);

			if (Quark::Input::IsMouseButtonPressed(Quark::MouseCode::Button3))
			{
				m_NormalLength -= elapsedTime;

				if (m_NormalLength < 0.001f)
					m_NormalLength = 0.001f;
			}

			if (Quark::Input::IsMouseButtonPressed(Quark::MouseCode::Button4))
			{
				m_NormalLength += elapsedTime;

				if (m_NormalLength > 1.0f)
					m_NormalLength = 1.0f;
			}
		}

		if (m_Perspective)
			m_CameraController.OnUpdate(elapsedTime);
		else
			m_OrthoCameraController.OnUpdate(elapsedTime);

		m_Scene.OnUpdate(elapsedTime);

		m_CoinAnimator.OnUpdate(elapsedTime);

		{
			// TODO: render pipeline and proper light object
			m_SelectedShader->Attach();
			m_SelectedShader->SetInt("u_Material.diffuse", 0);
			m_SelectedShader->SetInt("u_Material.specular", 1);
			m_SelectedShader->SetInt("u_Material.normalMap", 2);
			m_SelectedShader->SetInt("u_Cubemap", 3);
			m_SelectedShader->SetFloat("u_Material.shininess", 512.0f);
			m_SelectedShader->SetFloat("u_Material.metalness", 1.0f);

			constexpr float lightPower = 1.0f;
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

		m_ModelEntity.GetComponent<Quark::Transform3DComponent>().Rotate(elapsedTime * glm::radians(8.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		{
			m_SelectedShader->SetFloat("u_Material.metalness", 0.0f);
			Quark::Renderer::BeginScene(m_PortalCameraEntity.GetComponent<Quark::PerspectiveCameraComponent>().Camera.GetProjection(),
				m_PortalCameraEntity.GetComponent<Quark::Transform3DComponent>());

			m_Framebuffer->Attach();
			Quark::RenderCommand::SetViewport(0, 0, m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());

			// Drawing Suzanne to the framebuffer
			Quark::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			Quark::RenderCommand::Clear();

			Quark::Renderer::Submit(m_SelectedShader, m_ModelEntity.GetComponent<Quark::MeshComponent>().Mesh.GetVertexArray(), m_ModelEntity.GetComponent<Quark::Transform3DComponent>());

			m_Framebuffer->Detach();
			Quark::RenderCommand::SetViewport(0, 0, GetWindow().GetWidth(), GetWindow().GetHeight());

			Quark::Renderer::EndScene();
			m_SelectedShader->SetFloat("u_Material.metalness", 1.0f);
		}
		
		if (m_Perspective)
		{
			{
				auto& cameraTransform = m_CameraEntity.GetComponent<Quark::Transform3DComponent>();
				Quark::Renderer::BeginScene(m_CameraEntity.GetComponent<Quark::PerspectiveCameraComponent>().Camera.GetProjection(), cameraTransform);

				Quark::Renderer::SubmitSprite(m_CoinAnimator.GetTexture(), glm::mat4(1.0f));

				Quark::Renderer::Submit(m_SelectedShader, m_ModelEntity.GetComponent<Quark::MeshComponent>().Mesh.GetVertexArray(), m_ModelEntity.GetComponent<Quark::Transform3DComponent>());
				Quark::Renderer::Submit(m_SelectedShader, m_LightEntity.GetComponent<Quark::MeshComponent>().Mesh.GetVertexArray(), m_LightEntity.GetComponent<Quark::Transform3DComponent>());
				Quark::Renderer::Submit(m_SelectedShader, m_Framebuffer, m_PlaneEntity.GetComponent<Quark::MeshComponent>().Mesh.GetVertexArray(), m_PlaneEntity.GetComponent<Quark::Transform3DComponent>());

				Quark::Renderer::EndScene();
			}

			{
				glm::dmat4 view = m_CameraEntity.GetComponent<Quark::Transform3DComponent>();
				Quark::Renderer::BeginScene(m_CameraEntity.GetComponent<Quark::PerspectiveCameraComponent>().Camera.GetProjection(), view);

				Quark::RenderCommand::SetCullFace(Quark::RenderCullFace::Back);
				Quark::RenderCommand::SetDepthFunction(Quark::RenderDepthFunction::LessEqual);

				m_Skybox.GetCubeMap()->Attach(0);
				Quark::Renderer::Submit(m_ShaderLibrary.Get("skybox"), m_Skybox.GetVertexArray());

				Quark::RenderCommand::SetCullFace(Quark::RenderCullFace::Front);
				Quark::RenderCommand::SetDepthFunction(Quark::RenderDepthFunction::Less);

				Quark::Renderer::EndScene();
			}
		}
		else
		{
			auto& orthoCameraTransform = m_OrthoCameraEntity.GetComponent<Quark::Transform3DComponent>();
			Quark::Renderer::BeginScene(m_OrthoCameraEntity.GetComponent<Quark::OrthographicCameraComponent>().Camera.GetMatrix(), orthoCameraTransform);

			static Quark::Transform3DComponent transform;
			if (GetWindow().IsSelected())
			{
				transform.Position.x = Quark::Input::GetMouseX() * 0.01f;
				transform.Position.y = Quark::Input::GetMouseY() * 0.01f;
			}

			transform.Scale.x = (float)m_Maxwell->GetWidth() / (float)m_Maxwell->GetHeight();
			Quark::Renderer::SubmitSprite(m_Maxwell, (glm::dmat4)transform);

			constexpr int32_t scale = 100;
			static Quark::Transform3DComponent trans;
			for (int y = 0; y < scale; y++)
			{
				for (int x = 0; x < scale; x++)
				{	
					trans.Position = { y - scale * 0.5f, x - scale * 0.5f, 0.0f };
					//Quark::Renderer::SubmitSprite({ x * 0.01f, y * 0.01f, 0.0f, 1.0f }, trans);
					Quark::Renderer::SubmitSprite(m_Cobblestone, (glm::dmat4)trans);
				}
			}

			Quark::Renderer::EndScene();

			//std::cout << Entropy::Renderer::GetStats().DrawCalls << std::endl;
			//std::cout << Entropy::Renderer::GetStats().QuadsDrawn << std::endl;
		}
	}

	void OnDestroy() override
	{
		QK_TIME_SCOPE_DEBUG(SandboxApp::OnDestroy);

		m_Scene.DeleteEntity(m_PlaneEntity);
		m_Scene.DeleteEntity(m_ModelEntity);
		m_Scene.DeleteEntity(m_LightEntity);
		m_Scene.DeleteEntity(m_CameraEntity);
		m_Scene.DeleteEntity(m_OrthoCameraEntity);
		m_Scene.DeleteEntity(m_PortalCameraEntity);
	}

	void OnEvent(Quark::Event& e) override
	{
		Quark::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Quark::MouseButtonPressedEvent>(ATTACH_EVENT_FN(SandboxGame::OnMouseButtonPressed));
		dispatcher.Dispatch<Quark::KeyPressedEvent>(ATTACH_EVENT_FN(SandboxGame::OnKeyPressed));

		if (m_Perspective)
			m_CameraController.OnEvent(e);
		else
			m_OrthoCameraController.OnEvent(e);
	}

	bool OnMouseButtonPressed(Quark::MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
		case Quark::MouseCode::ButtonLeft:
		{
			// Focus enabled
			if (GetWindow().IsSelected())
				Quark::AudioEngine::PlaySound("assets/audio/bloop.wav");

			GetWindow().Select();
			break;
		}
		case Quark::MouseCode::ButtonRight:
		{
			// Focus enabled
			if (GetWindow().IsSelected())
				Quark::AudioEngine::PlaySound("assets/audio/bloop2.wav");
			break;
		}
		}
		return false;
	}

	bool OnKeyPressed(Quark::KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Quark::Key::F1:
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
			QK_CORE_TRACE(ss.str());
			break;
		}
		case Quark::Key::F2:
		{
			m_Perspective = !m_Perspective;
			break;
		}
		case Quark::Key::F3:
		{
			// Toggle VSync
			auto& window = GetWindow();
			window.SetVSync(!window.IsVSync());

			std::stringstream ss;
			ss << "VSync is now ";
			ss << (window.IsVSync() ? "enabled" : "disabled");
			QK_CORE_TRACE(ss.str());
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

			QK_TRACE("Switched environment");

			break;
		}
		*/
		case Quark::Key::F11:
		{
			auto& window = GetWindow();
			window.SetFullScreen(!window.IsFullscreen());
			break;
		}
		case Quark::Key::Escape:
		{
			// Focus lost
			GetWindow().Deselect();
			break;
		}
		}

		return false;
	}

private:
	Quark::Scene m_Scene;

	Quark::Entity m_PlaneEntity = m_Scene.CreateEntity();
	Quark::Entity m_ModelEntity = m_Scene.CreateEntity();
	Quark::Entity m_LightEntity = m_Scene.CreateEntity();
	Quark::Entity m_CameraEntity = m_Scene.CreateEntity();
	Quark::Entity m_OrthoCameraEntity = m_Scene.CreateEntity();
	Quark::Entity m_PortalCameraEntity = m_Scene.CreateEntity();

	Quark::Ref<Quark::Texture2D> m_DiffuseMap  = Quark::Texture2D::Create("./assets/textures/container.png");
	Quark::Ref<Quark::Texture2D> m_SpecularMap = Quark::Texture2D::Create("./assets/textures/container_specular.png");
	Quark::Ref<Quark::Texture2D> m_NormalMap   = Quark::Texture2D::Create("./assets/textures/normal_map.png");
	Quark::Ref<Quark::Texture2D> m_Cobblestone = Quark::Texture2D::Create("./assets/textures/cobblestone.png");
	Quark::Ref<Quark::Texture2D> m_CoinSheet   = Quark::Texture2D::Create("./assets/textures/coin.png");
	Quark::Ref<Quark::Texture2D> m_Maxwell     = Quark::Texture2D::Create("./assets/textures/Maxwell.png");

	Quark::SpriteAnimator m_CoinAnimator = { {
		{ m_CoinSheet, { 0, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 1, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 2, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 3, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 4, 0 }, { 16, 16 } },
		{ m_CoinSheet, { 5, 0 }, { 16, 16 } } }, 8.0f
	};

	Quark::ShaderLibrary m_ShaderLibrary;
	Quark::Ref<Quark::Shader> m_SelectedShader;
	float m_NormalLength = 0.1f;

	Quark::UIElement m_Element;

	Quark::Environment m_Skybox = { "./assets/environments/Lycksele3" };

	Quark::PerspectiveCameraController m_CameraController = { m_CameraEntity };
	Quark::OrthographicCameraController m_OrthoCameraController = { m_OrthoCameraEntity };

	// TODO: fix multisampling
	Quark::FramebufferSpecification m_FramebufferSpec = { 1024, 1024, 1, {
		{ Quark::TextureDataFormat::RGB8, Quark::TextureFilteringFormat::Linear, Quark::TextureTilingFormat::Repeat },
		{ Quark::TextureDataFormat::Depth24Stencil8 } } };
	Quark::Ref<Quark::Framebuffer> m_Framebuffer = Quark::Framebuffer::Create(m_FramebufferSpec);

	bool m_Perspective = false;
};

int main()
{
	auto app = new SandboxGame();
	app->Run();
	delete app;

	return 0;
}
